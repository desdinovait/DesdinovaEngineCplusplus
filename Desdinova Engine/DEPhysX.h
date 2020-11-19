#ifndef H_PHYSX_H
#define H_PHYSX_H


//*** Stream ***
class UserStream : public NxStream
{
public:
								UserStream(const char* filename, bool load);
	virtual						~UserStream();

	virtual		NxU8			readByte()								const;
	virtual		NxU16			readWord()								const;
	virtual		NxU32			readDword()								const;
	virtual		float			readFloat()								const;
	virtual		double			readDouble()							const;
	virtual		void			readBuffer(void* buffer, NxU32 size)	const;

	virtual		NxStream&		storeByte(NxU8 b);
	virtual		NxStream&		storeWord(NxU16 w);
	virtual		NxStream&		storeDword(NxU32 d);
	virtual		NxStream&		storeFloat(NxReal f);
	virtual		NxStream&		storeDouble(NxF64 f);
	virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size);

				FILE*			fp;
};

class MemoryWriteBuffer : public NxStream
{
	public:
								MemoryWriteBuffer();
	virtual						~MemoryWriteBuffer();

	virtual		NxU8			readByte()								const	{ NX_ASSERT(0);	return 0;	}
	virtual		NxU16			readWord()								const	{ NX_ASSERT(0);	return 0;	}
	virtual		NxU32			readDword()								const	{ NX_ASSERT(0);	return 0;	}
	virtual		float			readFloat()								const	{ NX_ASSERT(0);	return 0.0f;}
	virtual		double			readDouble()							const	{ NX_ASSERT(0);	return 0.0;	}
	virtual		void			readBuffer(void* buffer, NxU32 size)	const	{ NX_ASSERT(0);				}

	virtual		NxStream&		storeByte(NxU8 b);
	virtual		NxStream&		storeWord(NxU16 w);
	virtual		NxStream&		storeDword(NxU32 d);
	virtual		NxStream&		storeFloat(NxReal f);
	virtual		NxStream&		storeDouble(NxF64 f);
	virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size);

				NxU32			currentSize;
				NxU32			maxSize;
				NxU8*			data;
};

class MemoryReadBuffer : public NxStream
{
	public:
								MemoryReadBuffer(const NxU8* data);
	virtual						~MemoryReadBuffer();

	virtual		NxU8			readByte()								const;
	virtual		NxU16			readWord()								const;
	virtual		NxU32			readDword()								const;
	virtual		float			readFloat()								const;
	virtual		double			readDouble()							const;
	virtual		void			readBuffer(void* buffer, NxU32 size)	const;

	virtual		NxStream&		storeByte(NxU8 b)							{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeWord(NxU16 w)							{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeDword(NxU32 d)							{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeFloat(NxReal f)						{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeDouble(NxF64 f)						{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size)	{ NX_ASSERT(0);	return *this;	}

	mutable		const NxU8*		buffer;
};



enum UserDataFlag
{
	UD_IS_HW_ACTOR					= (1<<0),
	UD_IS_TRIGGER			        = (1<<1),
    UD_IS_INSIDE_TRIGGER            = (1<<2),
    UD_PASSES_INTERSECTION_TEST     = (1<<3),
    UD_HIT_BY_RAYCAST               = (1<<4),
    UD_NO_RENDER                    = (1<<5),
    UD_IS_DRAIN                     = (1<<6),
	UD_IS_ASLEEP				    = (1<<7),
	UD_RENDER_USING_LIGHT1			= (1<<8),
};


void AddUserDataToShapes(NxActor* actor, unsigned int id);
void ReleaseUserDataFromShapes(NxActor* actor);


class ActorUserData
{
public:
	NxU32 id;
	NxU32 contactEvents;
	NxU32 flags;
	NxU32 triggerID;

	ActorUserData()
	{
		id = 0;
		contactEvents = 0;
		flags = 0;
		triggerID = 0;
	}
};

class ShapeUserData
{
public:
	NxU32 id;
	void* mesh;
    void* model;

	NxReal wheelShapeRollAngle;
	NxMat34 wheelShapePose;

	ShapeUserData()
	{
		id = 0;
		mesh = NULL;
        model = NULL;
		wheelShapeRollAngle = 0;
//		wheelShapePose = 0;
	}
};









NxQuat AnglesToQuat(const NxVec3& angles);





//*** Contacts ***
class ContactPoint
{
public:
	ContactPoint(const NxVec3& _p, const NxVec3& _color)
	{
		p = _p;
		color = _color;
	}

	NxVec3	p;
	NxVec3	color;
};

class ContactLine
{
public:
	ContactLine(const NxVec3& _p0, const NxVec3& _p1, const NxVec3& _color)
	{
		p0 = _p0;
		p1 = _p1;
		color = _color;
	}

	NxVec3	p0;
	NxVec3	p1;
	NxVec3	color;
};

class ContactTriangle
{
public:
	ContactTriangle(const NxVec3& _p0, const NxVec3& _p1, const NxVec3& _p2, const NxVec3& _color)
	{
		p0 = _p0;
		p1 = _p1;
		p2 = _p2;
		color = _color;
	}

	NxVec3	p0;
	NxVec3	p1;
	NxVec3	p2;
	NxVec3	color;
};







/*** Reports ***
typedef NxArray<ContactPoint> ContactPointsArray;
typedef NxArray<ContactLine> ContactLinesArray;
typedef NxArray<ContactTriangle> ContactTrisArray;

ContactPointsArray cpArray;
ContactLinesArray clArray;
ContactTrisArray ctArray;*/

NxArray<ContactPoint> cpArray;
NxArray<ContactLine> clArray;
NxArray<ContactTriangle> ctArray;


class gContactReport : public NxUserContactReport
{
public:
	DLL_API virtual void onContactNotify(NxContactPair& pair, NxU32 events);
} gContactReport;


class gTriggerReport : public NxUserTriggerReport
{
public:
	DLL_API virtual void onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status);
} gTriggerReport;


class gShapeReport : public NxUserEntityReport<NxShape*>
{
public:
	DLL_API virtual bool onEvent(NxU32 nbShapes, NxShape** shapes);
} gShapeReport;


class gRaycastReport : public NxUserRaycastReport
{
public:
    DLL_API virtual bool onHit(const NxRaycastHit& hit);
} gRaycastReport;



#endif
