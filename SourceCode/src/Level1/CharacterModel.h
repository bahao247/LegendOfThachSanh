#pragma once
#include "Ogre.h"
#include "OIS.h"
#include "SdkGame.h"
#include "SdkTableCommon.h"

using namespace Ogre;

class CharacterModel
{
public:
    CharacterModel(void);
    CharacterModel(Camera* cam);
    ~CharacterModel(void);
    Vector3 getGoalDirection();
    Real getLeng();
    void setCharacterPos(Vector3 characterPos);
    Vector3 getCharacterPos();
    void setMinPos(bool MinPos);
    int getAnime();
    bool checkJump();
    Real getSpeed();
    void setSpeed(Real Speed);
    int getHP();
    void setHP(int Health);
    void setScale(Real Scale);
    void setDead(void);
    void addTime(Real deltaTime);
    void setWinAnime();
    void injectKeyDown(const OIS::KeyEvent& evt);
    void setupBody(SceneManager* sceneMgr, Vector3 camPost);
    void setupCamera(Camera* cam);
    void updateBody(Real deltaTime);
    void fadeAnimations(Real deltaTime);
    void updateCamera(Real deltaTime);
    void updateCameraGoal(Real deltaYaw, Real deltaPitch, Real deltaZoom);
    void setBaseAnimation(AnimID id, bool reset = false);
    void setTopAnimation(AnimID id, bool reset = false);
private:
    SceneNode* mBodyNode;
    SceneNode* mCameraPivot;
    SceneNode* mCameraGoal;
    SceneNode* mCameraNode;
    Real mPivotPitch;
    Entity* mBodyEnt;
    Entity* mSword1;
    Entity* mSword2;
    RibbonTrail* mSwordTrail;
    AnimationState* mAnims[NUM_ANIMS];    // master animation list
    AnimID mBaseAnimID;                   // current base (full- or lower-body) animation
    AnimID mTopAnimID;                    // current top (upper-body) animation
    bool mFadingIn[NUM_ANIMS];            // which animations are fading in
    bool mFadingOut[NUM_ANIMS];           // which animations are fading out
    bool mSwordsDrawn;
    Vector3 mKeyDirection;      // player's local intended direction based on WASD keys
    Vector3 mGoalDirection;     // actual intended direction in world-space
    Vector3 mSaveGoalDirection;     // save actual intended direction in world-space
    Real mVerticalVelocity;     // for jumping
    Real mTimer;                // general timer to see how long animations have been playing
    bool mMinPos; //Min height Terrain at pos

    Real mMechLength;
    Real mSpeed;
    Real mScale;
    int mHP;
};

