#pragma once

#include "../../GcLib/pch.h"

#include "DnhCommon.hpp"
#include "DnhGcLibImpl.hpp"
#include "DnhReplay.hpp"
#include "DnhScript.hpp"

class StgSystemController;
class StgSystemInformation;
class StgStageController;
class StgPackageController;
class StgStageInformation;
class StgSystemInformation;
class StgMovePattern;
class StgMoveParent;

//*******************************************************************
//StgMoveObject
//*******************************************************************
class StgMoveObject {
	friend StgMovePattern;
	friend StgMoveParent;
private:
	StgStageController* stageController_;
protected:
	double posX_;
	double posY_;

	ref_unsync_ptr<StgMovePattern> pattern_;

	bool bEnableMovement_;

	ref_unsync_weak_ptr<StgMoveParent> parent_;
	double offX_;
	double offY_;
	std::vector<ref_unsync_weak_ptr<StgMoveParent>> listOwnedParent_;

	int framePattern_;
	std::map<int, std::list<ref_unsync_ptr<StgMovePattern>>> mapPattern_;
	virtual void _Move();
	void _AttachReservedPattern(ref_unsync_ptr<StgMovePattern> pattern);
public:
	StgMoveObject(StgStageController* stageController);
	virtual ~StgMoveObject();

	void Move();

	void SetEnableMovement(bool b) { bEnableMovement_ = b; }
	bool IsEnableMovement() { return bEnableMovement_; }

	double GetPositionX() { return posX_; }
	void SetPositionX(double pos) { posX_ = pos; }
	double GetPositionY() { return posY_; }
	void SetPositionY(double pos) { posY_ = pos; }

	double GetSpeed();
	void SetSpeed(double speed);
	double GetDirectionAngle();
	void SetDirectionAngle(double angle);

	double GetSpeedX();
	double GetSpeedY();
	void SetSpeedX(double speedX);
	void SetSpeedY(double speedY);

	void SetParent(ref_unsync_weak_ptr<StgMoveParent> parent) { parent_ = parent; }
	ref_unsync_weak_ptr<StgMoveParent> GetParent() { return parent_; }
	void RemoveParent() { parent_ = nullptr; }
	void SetRelativePosition(float x, float y) { offX_ = x; offY_ = y; }
	void UpdateRelativePosition();

	ref_unsync_ptr<StgMovePattern> GetPattern() { return pattern_; }
	void SetPattern(ref_unsync_ptr<StgMovePattern> pattern) {
		pattern_ = pattern;
	}
	void AddPattern(int frameDelay, ref_unsync_ptr<StgMovePattern> pattern, bool bForceMap = false);
};

//*******************************************************************
//StgMoveParent
//*******************************************************************
class StgMoveParent : public DxScriptObjectBase {
	friend StgMoveObject;
public:
	enum {
		ANGLE_FIXED,		// Don't change the angle at all
		ANGLE_ROTATE,		// Increment angle only when transform angle is changed
		ANGLE_FOLLOW,		// Match angle to that of the target move object (same as ANGLE_FIXED if no target is set)
		ANGLE_ABSOLUTE,		// Get absolute angle from last position
		ANGLE_RELATIVE,		// Get angle from last position relative to base point
		ANGLE_OUTWARD,		// Face outwards from base point
		ANGLE_INWARD,		// Face inwards from base point
	};
	enum {
		ORDER_ANGLE_SCALE,
		ORDER_SCALE_ANGLE
	};
private:
	StgStageController* stageController_;
protected:
	ref_unsync_weak_ptr<StgMoveObject> target_;
	int typeAngle_;
	int transOrder_;
	bool bAutoDelete_;
	bool bMoveChild_;
	bool bRotateLaser_;

	double posX_;
	double posY_;
	double offX_;
	double offY_;
	double scaX_;
	double scaY_;
	double rotZ_;
	std::vector<ref_unsync_weak_ptr<StgMoveObject>> listChild_;

public:
	StgMoveParent(StgStageController* stageController);
	~StgMoveParent();

	virtual void Work();
	virtual void Render() {}
	virtual void SetRenderState() {}
	virtual void CleanUp();

	void SetParentObject(ref_unsync_weak_ptr<StgMoveParent> self, ref_unsync_weak_ptr<StgMoveObject> parent);
	ref_unsync_weak_ptr<StgMoveParent> GetParentObject() { return target_;  }
	void SetAutoDelete(bool enable) { bAutoDelete_ = enable; }
	void AddChild(ref_unsync_weak_ptr<StgMoveParent> self, ref_unsync_weak_ptr<StgMoveObject> child);
	std::vector<ref_unsync_weak_ptr<StgMoveObject>>& GetChildren() { return listChild_; }
	void RemoveChildren();
	
	void SetPositionOffset(double x, double y) { offX_ = x; offY_ = y; }
	void SetTransformScale(double x, double y) { scaX_ = unzero(x); scaY_ = unzero(y); }
	void SetTransformScaleX(double x) { scaX_ = unzero(x); }
	void SetTransformScaleY(double y) { scaY_ = unzero(y); }
	void SetTransformAngle(double z);
	double GetTransformScaleX() { return scaX_; }
	double GetTransformScaleY() { return scaY_; }
	double GetTransformAngle() { return rotZ_; }
	void SetChildAngleMode(int type) { typeAngle_ = type; }
	int GetChildAngleMode() { return typeAngle_;  }
	void SetChildMotionEnable(bool enable) { bMoveChild_ = enable; }
	void SetLaserRotationEnable(bool enable) { bRotateLaser_ = enable; }
	void SetTransformOrder(int order) { transOrder_ = order; }
	void ApplyTransformation();
	void ResetTransformation() { scaX_ = 1; scaY_ = 1; rotZ_ = 0; }
	
	inline void SetPosition(double x, double y) { posX_ = x; posY_ = y; }
	inline double GetX() { return posX_ + offX_; }
	inline double GetY() { return posY_ + offY_; }
	void MoveChild(StgMoveObject* child);

	void UpdateChildren();

	static inline double unzero(double s) {
		return (s >= 0) ? std::max(s, 0.00001) : std::min(s, -0.00001);
	}
};

//*******************************************************************
//StgMovePattern
//*******************************************************************
class StgMovePattern {
	friend StgMoveObject;
public:
	enum {
		TYPE_OTHER,
		TYPE_ANGLE,
		TYPE_XY,
		TYPE_LINE,

		NO_CHANGE = -0x1000000,
		TOPLAYER_CHANGE = 0x1000000,
		SET_ZERO = -1,
	};
protected:
	int typeMove_;
	StgMoveObject* target_;

	int frameWork_;
	int idShotData_;

	double c_;
	double s_;

	std::list<std::pair<int8_t, double>> listCommand_;

	StgStageController* _GetStageController() { return target_->stageController_; }
	ref_unsync_ptr<StgMoveObject> _GetMoveObject(int id);
	virtual void _Activate(StgMovePattern* src) {}
public:
	StgMovePattern(StgMoveObject* target);
	virtual ~StgMovePattern() {}

	virtual void Move() = 0;

	void AddCommand(std::pair<uint8_t, double> cmd) { listCommand_.push_back(cmd); }
	int GetType() { return typeMove_; }

	virtual inline double GetSpeed() = 0;
	virtual inline double GetDirectionAngle() = 0;
	int GetShotDataID() { return idShotData_; }
	void SetShotDataID(int id) { idShotData_ = id; }

	virtual double GetSpeedX() { return c_; }
	virtual double GetSpeedY() { return s_; }
};

class StgMovePattern_XY;
class StgMovePattern_Angle : public StgMovePattern {
	friend class StgMoveObject;
	friend class StgMovePattern_XY;
public:
	enum : int8_t {
		SET_SPEED,
		SET_ANGLE,
		SET_ACCEL,
		SET_AGVEL,
		SET_SPMAX,
		SET_SPMAX2,
		ADD_SPEED,
		ADD_ANGLE,
		ADD_ACCEL,
		ADD_AGVEL,
		ADD_SPMAX,
	};
protected:
	double speed_;
	double angDirection_;
	double acceleration_;
	double maxSpeed_;
	double angularVelocity_;

	ref_unsync_weak_ptr<StgMoveObject> objRelative_;

	virtual void _Activate(StgMovePattern* src);
public:
	StgMovePattern_Angle(StgMoveObject* target);
	virtual void Move();

	virtual inline double GetSpeed() { return speed_; }
	virtual inline double GetDirectionAngle() { return angDirection_; }

	void SetSpeed(double speed) { speed_ = speed; }
	void SetDirectionAngle(double angle);
	void SetAcceleration(double accel) { acceleration_ = accel; }
	void SetMaxSpeed(double max) { maxSpeed_ = max; }
	void SetAngularVelocity(double av) { angularVelocity_ = av; }

	void SetRelativeObject(ref_unsync_weak_ptr<StgMoveObject> obj) { objRelative_ = obj; }
	void SetRelativeObject(int id) { objRelative_ = _GetMoveObject(id); }

	virtual inline double GetSpeedX() {
		return (speed_ * c_);
	}
	virtual inline double GetSpeedY() {
		return (speed_ * s_);
	}
};

class StgMovePattern_XY : public StgMovePattern {
	friend class StgMoveObject;
	friend class StgMovePattern_Angle;
public:
	enum : int8_t {
		SET_S_X,
		SET_S_Y,
		SET_A_X,
		SET_A_Y,
		SET_M_X,
		SET_M_Y,
	};
protected:
	double accelerationX_;
	double accelerationY_;
	double maxSpeedX_;
	double maxSpeedY_;

	virtual void _Activate(StgMovePattern* src);
public:
	StgMovePattern_XY(StgMoveObject* target);

	virtual void Move();

	virtual inline double GetSpeed() { return hypot(c_, s_); }
	virtual inline double GetDirectionAngle() { return atan2(s_, c_); }

	virtual double GetSpeedX() { return c_; }
	virtual double GetSpeedY() { return s_; }
	void SetSpeedX(double value) { c_ = value; }
	void SetSpeedY(double value) { s_ = value; }
	void SetAccelerationX(double value) { accelerationX_ = value; }
	void SetAccelerationY(double value) { accelerationY_ = value; }
	void SetMaxSpeedX(double value) { maxSpeedX_ = value; }
	void SetMaxSpeedY(double value) { maxSpeedY_ = value; }

	double GetAccelerationX() { return accelerationX_; }
	double GetAccelerationY() { return accelerationY_; }
	double GetMaxSpeedX() { return maxSpeedX_; }
	double GetMaxSpeedY() { return maxSpeedY_; }
};

class StgMovePattern_Line : public StgMovePattern {
	friend class StgMoveObject;
protected:
	enum {
		TYPE_SPEED,
		TYPE_FRAME,
		TYPE_WEIGHT,
		TYPE_NONE,
	};

	int typeLine_;
	int maxFrame_;
	double speed_;
	double angDirection_;
	
	double iniPos_[2];
	double targetPos_[2];
public:
	StgMovePattern_Line(StgMoveObject* target);

	virtual void Move();

	virtual inline double GetSpeed() { return speed_; }
	virtual inline double GetDirectionAngle() { return angDirection_; }

	virtual double GetSpeedX() { return (speed_ * c_); }
	virtual double GetSpeedY() { return (speed_ * s_); }
};
class StgMovePattern_Line_Speed : public StgMovePattern_Line {
	friend class StgMoveObject;
public:
	StgMovePattern_Line_Speed(StgMoveObject* target);

	void SetAtSpeed(double tx, double ty, double speed);
};
class StgMovePattern_Line_Frame : public StgMovePattern_Line {
	friend class StgMoveObject;
public:
	using lerp_func = Math::Lerp::funcLerp<double, double>;
	using lerp_diff_func = Math::Lerp::funcLerpDiff<double>;
protected:
	double speedRate_;
	lerp_func moveLerpFunc;
	lerp_diff_func diffLerpFunc;
public:
	StgMovePattern_Line_Frame(StgMoveObject* target);

	virtual void Move();

	void SetAtFrame(double tx, double ty, int frame, lerp_func lerpFunc, lerp_diff_func diffFunc);
};
class StgMovePattern_Line_Weight : public StgMovePattern_Line {
	friend class StgMoveObject;
protected:
	double dist_;
	double weight_;
	double maxSpeed_;
public:
	StgMovePattern_Line_Weight(StgMoveObject* target);

	virtual void Move();

	void SetAtWeight(double tx, double ty, double weight, double maxSpeed);
};