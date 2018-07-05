#include "CharacterModel.h"


CharacterModel::CharacterModel(void)
{
}


CharacterModel::CharacterModel(Camera* cam) : mSpeed(RUN_SPEED)
    , mScale(1)
    , mHP(100)
{

}

CharacterModel::~CharacterModel(void)
{
}

Ogre::Vector3 CharacterModel::getGoalDirection()
{
    return (mSaveGoalDirection);
}

Ogre::Real CharacterModel::getLeng()
{
    return mMechLength;
}

void CharacterModel::setCharacterPos(Vector3 characterPos)
{
    mBodyNode->setPosition(characterPos);
}

Ogre::Vector3 CharacterModel::getCharacterPos()
{
    Vector3 characterPos = mBodyNode->getPosition();
    return characterPos;
}

void CharacterModel::setMinPos(bool MinPos)
{
    mMinPos = MinPos;
}

int CharacterModel::getAnime()
{
    return mBaseAnimID;
}

bool CharacterModel::checkJump()
{
    return (mVerticalVelocity < 0);
}

Ogre::Real CharacterModel::getSpeed()
{
    return mSpeed;
}

void CharacterModel::setSpeed(Real Speed)
{
    mSpeed = Speed;
}

int CharacterModel::getHP()
{
    return mHP;
}

void CharacterModel::setHP(int Health)
{
    mHP = Health;
}

void CharacterModel::setScale(Real Scale)
{
    mScale = Scale;
}

void CharacterModel::setDead(void)
{
    mBodyNode->translate(0.0f, mBodyEnt->getBoundingBox().getHalfSize().y*mScale, 0.0f);
    mBodyNode->pitch(Degree(90));
    mSpeed=0;
}

void CharacterModel::addTime(Real deltaTime)
{
    updateBody(deltaTime);
    if (mHP)
    {
        updateAnimations(deltaTime);
    }
    updateCamera(deltaTime);
}

void CharacterModel::setWinAnime()
{
    mSpeed = 0;
    if (mSwordsDrawn)
    {
        setTopAnimation(ANIM_DRAW_SWORDS, true);
        mTimer = 0;
    }

    if (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP)
    {
        // start dancing
        setBaseAnimation(ANIM_DANCE, true);
        setTopAnimation(ANIM_NONE);
        // disable hand animation because the dance controls hands
        mAnims[ANIM_HANDS_RELAXED]->setEnabled(false);
    }
    else if (mBaseAnimID == ANIM_DANCE)
    {
        // stop dancing
        setBaseAnimation(ANIM_IDLE_BASE);
        setTopAnimation(ANIM_IDLE_TOP);
        // re-enable hand animation
        mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
    }
}

void CharacterModel::setupBody(SceneManager* sceneMgr, Vector3 camPost)
{
    // create main model
    mBodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
    mBodyEnt = sceneMgr->createEntity("SinbadBody", "Sinbad.mesh");
    mBodyNode->setScale(mScale, mScale, mScale);
    mBodyNode->attachObject(mBodyEnt);

    Ogre::AxisAlignedBox box = mBodyEnt->getBoundingBox();

    mMechLength = box.getSize().x;

    // create swords and attach to sheath
    LogManager::getSingleton().logMessage("Creating swords");
    mSword1 = sceneMgr->createEntity("SinbadSword1", "Sword.mesh");
    mSword2 = sceneMgr->createEntity("SinbadSword2", "Sword.mesh");
    mBodyEnt->attachObjectToBone("Sheath.L", mSword1);
    mBodyEnt->attachObjectToBone("Sheath.R", mSword2);

    LogManager::getSingleton().logMessage("Creating the chains");
    // create a couple of ribbon trails for the swords, just for fun
    NameValuePairList params;
    params["numberOfChains"] = "2";
    params["maxElements"] = "80";
    mSwordTrail = (RibbonTrail*)sceneMgr->createMovableObject("RibbonTrail", &params);
    mSwordTrail->setMaterialName("Examples/LightRibbonTrail");
    mSwordTrail->setTrailLength(20);
    mSwordTrail->setVisible(false);
    sceneMgr->getRootSceneNode()->attachObject(mSwordTrail);


    for (int i = 0; i < 2; i++)
    {
        mSwordTrail->setInitialColour(i, 1, 0.8, 0);
        mSwordTrail->setColourChange(i, 0.75, 1.25, 1.25, 1.25);
        mSwordTrail->setWidthChange(i, 1);
        mSwordTrail->setInitialWidth(i, 0.5);
    }

    mKeyDirection = Vector3::ZERO;
    mVerticalVelocity = 0;
}

void CharacterModel::setupCamera(Camera* cam)
{
    // create a pivot at roughly the character's shoulder
    mCameraPivot = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
    // this is where the camera should be soon, and it spins around the pivot
    mCameraGoal = mCameraPivot->createChildSceneNode(Vector3(0, 0, 15));
    // this is where the camera actually is
    mCameraNode = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
    mCameraNode->setPosition(mCameraPivot->getPosition() + mCameraGoal->getPosition());

    mCameraPivot->setFixedYawAxis(true);
    mCameraGoal->setFixedYawAxis(true);
    mCameraNode->setFixedYawAxis(true);

    // our model is quite small, so reduce the clipping planes
    //cam->setNearClipDistance(0.1);
    cam->setFarClipDistance(100);
    cam->setFarClipDistance(50000);
    mCameraNode->attachObject(cam);

    mPivotPitch = 0;
}

void CharacterModel::updateBody(Real deltaTime)
{
    if (mGoalDirection != Vector3::ZERO)
    {
        mSaveGoalDirection = mGoalDirection;
    }

    mGoalDirection = Vector3::ZERO;   // we will calculate this

    if (mKeyDirection != Vector3::ZERO && mBaseAnimID != ANIM_DANCE)
    {
        // calculate actually goal direction in world based on player's key directions
        mGoalDirection += mKeyDirection.z * mCameraNode->getOrientation().zAxis();
        mGoalDirection += mKeyDirection.x * mCameraNode->getOrientation().xAxis();
        mGoalDirection.y = 0;
        mGoalDirection.normalise();

        Quaternion toGoal = mBodyNode->getOrientation().zAxis().getRotationTo(mGoalDirection);

        // calculate how much the character has to turn to face goal direction
        Real yawToGoal = toGoal.getYaw().valueDegrees();
        // this is how much the character CAN turn this frame
        Real yawAtSpeed = yawToGoal / Math::Abs(yawToGoal) * deltaTime * TURN_SPEED;
        // reduce "turnability" if we're in midair
        if (mBaseAnimID == ANIM_JUMP_LOOP) yawAtSpeed *= 0.2f;

        // turn as much as we can, but not more than we need to
        if (yawToGoal < 0) yawToGoal = std::min<Real>(0, std::max<Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, yawAtSpeed, 0);
        else if (yawToGoal > 0) yawToGoal = std::max<Real>(0, std::min<Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, 0, yawAtSpeed);

        mBodyNode->yaw(Degree(yawToGoal));

        // move in current body direction (not the goal direction)
        mBodyNode->translate(0, 0, deltaTime * mSpeed * mAnims[mBaseAnimID]->getWeight(),
            Node::TS_LOCAL);
    }

    if (mBaseAnimID == ANIM_JUMP_LOOP)
    {
        // if we're jumping, add a vertical offset too, and apply gravity
        mBodyNode->translate(0, mVerticalVelocity * deltaTime, 0, Node::TS_LOCAL);
        mVerticalVelocity -= GRAVITY * deltaTime;

        Vector3 pos = mBodyNode->getPosition();
        if (mMinPos)
        {
            // if we've hit the ground, change to landing state
            //pos.y = CHAR_HEIGHT;
            //mBodyNode->setPosition(pos);
            setBaseAnimation(ANIM_JUMP_END, true);
            mTimer = 0;
            mMinPos = false;
            mVerticalVelocity = 0;
        }
    }
}

void CharacterModel::fadeAnimations(Real deltaTime)
{
    for (int i = 0; i < NUM_ANIMS; i++)
    {
        if (mFadingIn[i])
        {
            // slowly fade this animation in until it has full weight
            Real newWeight = mAnims[i]->getWeight() + deltaTime * ANIM_FADE_SPEED;
            mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
            if (newWeight >= 1) mFadingIn[i] = false;
        }
        else if (mFadingOut[i])
        {
            // slowly fade this animation out until it has no weight, and then disable it
            Real newWeight = mAnims[i]->getWeight() - deltaTime * ANIM_FADE_SPEED;
            mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
            if (newWeight <= 0)
            {
                mAnims[i]->setEnabled(false);
                mFadingOut[i] = false;
            }
        }
    }
}

void CharacterModel::updateCamera(Real deltaTime)
{
    // place the camera pivot roughly at the character's shoulder
    mCameraPivot->setPosition(mBodyNode->getPosition() + Vector3::UNIT_Y * CAM_HEIGHT);
    // move the camera smoothly to the goal
    Vector3 goalOffset = mCameraGoal->_getDerivedPosition() - mCameraNode->getPosition();
    mCameraNode->translate(goalOffset * deltaTime * 9.0f);
    // always look at the pivot
    mCameraNode->lookAt(mCameraPivot->_getDerivedPosition(), Node::TS_WORLD);
}

void CharacterModel::updateCameraGoal(Real deltaYaw, Real deltaPitch, Real deltaZoom)
{
    mCameraPivot->yaw(Degree(deltaYaw), Node::TS_WORLD);

    // bound the pitch
    if (!(mPivotPitch + deltaPitch > 25 && deltaPitch > 0) &&
        !(mPivotPitch + deltaPitch < -60 && deltaPitch < 0))
    {
        mCameraPivot->pitch(Degree(deltaPitch), Node::TS_LOCAL);
        mPivotPitch += deltaPitch;
    }

    Real dist = mCameraGoal->_getDerivedPosition().distance(mCameraPivot->_getDerivedPosition());
    Real distChange = deltaZoom * dist;

    // bound the zoom
    if (!(dist + distChange < 8 && distChange < 0) &&
        !(dist + distChange > 25 && distChange > 0))
    {
        mCameraGoal->translate(0, 0, distChange, Node::TS_LOCAL);
    }
}

void CharacterModel::setBaseAnimation(AnimID id, bool reset /*= false*/)
{
    if (mBaseAnimID >= 0 && mBaseAnimID < NUM_ANIMS)
    {
        // if we have an old animation, fade it out
        mFadingIn[mBaseAnimID] = false;
        mFadingOut[mBaseAnimID] = true;
    }

    mBaseAnimID = id;

    if (id != ANIM_NONE)
    {
        // if we have a new animation, enable it and fade it in
        mAnims[id]->setEnabled(true);
        mAnims[id]->setWeight(0);
        mFadingOut[id] = false;
        mFadingIn[id] = true;
        if (reset) mAnims[id]->setTimePosition(0);
    }
}

void CharacterModel::setTopAnimation(AnimID id, bool reset /*= false*/)
{
    if (mTopAnimID >= 0 && mTopAnimID < NUM_ANIMS)
    {
        // if we have an old animation, fade it out
        mFadingIn[mTopAnimID] = false;
        mFadingOut[mTopAnimID] = true;
    }

    mTopAnimID = id;

    if (id != ANIM_NONE)
    {
        // if we have a new animation, enable it and fade it in
        mAnims[id]->setEnabled(true);
        mAnims[id]->setWeight(0);
        mFadingOut[id] = false;
        mFadingIn[id] = true;
        if (reset) mAnims[id]->setTimePosition(0);
    }
}


