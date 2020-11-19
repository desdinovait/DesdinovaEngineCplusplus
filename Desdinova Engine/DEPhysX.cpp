#include "DEGeneralIncludes.h"

UserStream::UserStream(const char* filename, bool load) : fp(NULL)
{
	fp = fopen(filename, load ? "rb" : "wb");
}

UserStream::~UserStream()
{
	if(fp)	fclose(fp);
}

// Loading API
NxU8 UserStream::readByte() const
{
	NxU8 b;
	size_t r = fread(&b, sizeof(NxU8), 1, fp);
	NX_ASSERT(r);
	return b;
}

NxU16 UserStream::readWord() const
{
	NxU16 w;
	size_t r = fread(&w, sizeof(NxU16), 1, fp);
	NX_ASSERT(r);
	return w;
}

NxU32 UserStream::readDword() const
{
	NxU32 d;
	size_t r = fread(&d, sizeof(NxU32), 1, fp);
	NX_ASSERT(r);
	return d;
}

float UserStream::readFloat() const
{
	NxReal f;
	size_t r = fread(&f, sizeof(NxReal), 1, fp);
	NX_ASSERT(r);
	return f;
}

double UserStream::readDouble() const
{
	NxF64 f;
	size_t r = fread(&f, sizeof(NxF64), 1, fp);
	NX_ASSERT(r);
	return f;
}

void UserStream::readBuffer(void* buffer, NxU32 size)	const
{
	size_t w = fread(buffer, size, 1, fp);
	NX_ASSERT(w);
}

// Saving API
NxStream& UserStream::storeByte(NxU8 b)
{
	size_t w = fwrite(&b, sizeof(NxU8), 1, fp);
	NX_ASSERT(w);
	return *this;
}

NxStream& UserStream::storeWord(NxU16 w)
{
	size_t ww = fwrite(&w, sizeof(NxU16), 1, fp);
	NX_ASSERT(ww);
	return *this;
}

NxStream& UserStream::storeDword(NxU32 d)
{
	size_t w = fwrite(&d, sizeof(NxU32), 1, fp);
	NX_ASSERT(w);
	return *this;
}

NxStream& UserStream::storeFloat(NxReal f)
{
	size_t w = fwrite(&f, sizeof(NxReal), 1, fp);
	NX_ASSERT(w);
	return *this;
}

NxStream& UserStream::storeDouble(NxF64 f)
{
	size_t w = fwrite(&f, sizeof(NxF64), 1, fp);
	NX_ASSERT(w);
	return *this;
}

NxStream& UserStream::storeBuffer(const void* buffer, NxU32 size)
{
	size_t w = fwrite(buffer, size, 1, fp);
	NX_ASSERT(w);
	return *this;
}



MemoryWriteBuffer::MemoryWriteBuffer() : currentSize(0), maxSize(0), data(NULL)
{
}

MemoryWriteBuffer::~MemoryWriteBuffer()
{
	NxGetPhysicsSDKAllocator()->free(data);
}

NxStream& MemoryWriteBuffer::storeByte(NxU8 b)
{
	storeBuffer(&b, sizeof(NxU8));
	return *this;
}

NxStream& MemoryWriteBuffer::storeWord(NxU16 w)
{
	storeBuffer(&w, sizeof(NxU16));
	return *this;
}

NxStream& MemoryWriteBuffer::storeDword(NxU32 d)
{
	storeBuffer(&d, sizeof(NxU32));
	return *this;
}

NxStream& MemoryWriteBuffer::storeFloat(NxReal f)
{
	storeBuffer(&f, sizeof(NxReal));
	return *this;
}

NxStream& MemoryWriteBuffer::storeDouble(NxF64 f)
{
	storeBuffer(&f, sizeof(NxF64));
	return *this;
}

NxStream& MemoryWriteBuffer::storeBuffer(const void* buffer, NxU32 size)
{
	NxU32 expectedSize = currentSize + size;
	if(expectedSize > maxSize)
	{
		maxSize = expectedSize + 4096;

		NxU8* newData = (NxU8*)NxGetPhysicsSDKAllocator()->malloc(maxSize, NX_MEMORY_PERSISTENT);
		if(data)
		{
			memcpy(newData, data, currentSize);
			NxGetPhysicsSDKAllocator()->free(data);
		}
		data = newData;
	}
	memcpy(data+currentSize, buffer, size);
	currentSize += size;
	return *this;
}

MemoryReadBuffer::MemoryReadBuffer(const NxU8* data) : buffer(data)
{
}

MemoryReadBuffer::~MemoryReadBuffer()
{
	// We don't own the data => no delete
}

NxU8 MemoryReadBuffer::readByte() const
{
	NxU8 b;
	memcpy(&b, buffer, sizeof(NxU8));
	buffer += sizeof(NxU8);
	return b;
}

NxU16 MemoryReadBuffer::readWord() const
{
	NxU16 w;
	memcpy(&w, buffer, sizeof(NxU16));
	buffer += sizeof(NxU16);
	return w;
}

NxU32 MemoryReadBuffer::readDword() const
{
	NxU32 d;
	memcpy(&d, buffer, sizeof(NxU32));
	buffer += sizeof(NxU32);
	return d;
}

float MemoryReadBuffer::readFloat() const
{
	float f;
	memcpy(&f, buffer, sizeof(float));
	buffer += sizeof(float);
	return f;
}

double MemoryReadBuffer::readDouble() const
{
	double f;
	memcpy(&f, buffer, sizeof(double));
	buffer += sizeof(double);
	return f;
}

void MemoryReadBuffer::readBuffer(void* dest, NxU32 size) const
{
	memcpy(dest, buffer, size);
	buffer += size;
}














void AddUserDataToShapes(NxActor* actor, unsigned int id)
{
    NxU32 i = 0;

	NxShape*const* shapes = actor->getShapes();
	NxU32 nbShapes = actor->getNbShapes();
	while (nbShapes--)
	{
		NxShape* shape = shapes[nbShapes];
		NxShapeType type;
        shape->userData = new ShapeUserData;
		ShapeUserData* sud = (ShapeUserData*)(shape->userData);
		sud->id = id;
		type = shape->getType();
		if ( type == NX_SHAPE_CONVEX)
		{
			sud->mesh = new NxConvexMeshDesc;
			shape->isConvexMesh()->getConvexMesh().saveToDesc(*(NxConvexMeshDesc*)sud->mesh);
		}
		if ( type == NX_SHAPE_MESH)
		{
			sud->mesh = new NxTriangleMeshDesc;
			shape->isTriangleMesh()->getTriangleMesh().saveToDesc(*(NxTriangleMeshDesc*)sud->mesh);
		}
		/*if ( type == NX_SHAPE_BOX)
		{
			sud->mesh = new NxBoxShapeDesc;
			shape->isBox()->saveToDesc(*(NxBoxShapeDesc*)sud->mesh);
		}
		if ( type == NX_SHAPE_SPHERE)
		{
			sud->mesh = new NxSphereShapeDesc;
			shape->isSphere()->saveToDesc(*(NxSphereShapeDesc*)sud->mesh);
		}	*/	 
	}
}


void ReleaseUserDataFromShapes(NxActor* actor)
{
	NxShape*const* shapes = actor->getShapes();
	NxU32 nbShapes = actor->getNbShapes();
	while (nbShapes--)
	{
		NxShape* shape = shapes[nbShapes];
        if (shape->userData)
		{
		    ShapeUserData* sud = (ShapeUserData*)(shape->userData);
			if (sud && sud->mesh)
			{
			    delete sud->mesh;
		    }
		}
	}
}





NxQuat AnglesToQuat(const NxVec3& angles)
{
	NxQuat quat;
	NxVec3 a;
	NxReal cr, cp, cy, sr, sp, sy, cpcy, spsy;

	a.x = (NxPi/(NxReal)360.0) * angles.x;    // Pitch
	a.y = (NxPi/(NxReal)360.0) * angles.y;    // Yaw
	a.z = (NxPi/(NxReal)360.0) * angles.z;    // Roll

	cy = NxMath::cos(a.z);
	cp = NxMath::cos(a.y);
	cr = NxMath::cos(a.x);

	sy = NxMath::sin(a.z);
	sp = NxMath::sin(a.y);
	sr = NxMath::sin(a.x);

	cpcy = cp * cy;
	spsy = sp * sy;
	quat.w = cr * cpcy + sr * spsy;		
	quat.x = sr * cpcy - cr * spsy;		
	quat.y = cr * sp * cy + sr * cp * sy;	
	quat.z = cr * cp * sy - sr * sp * cy;

	return quat;
}










DLL_API void gContactReport::onContactNotify(NxContactPair& pair, NxU32 events)
{
	if (pair.actors[0])
	{
		ActorUserData* ud = (ActorUserData*)pair.actors[0]->userData;
		if (ud)  ud->contactEvents = events;
	}

	if (pair.actors[1])
	{
		ActorUserData* ud = (ActorUserData*)pair.actors[1]->userData;
		if (ud)  ud->contactEvents = events;
	}

//		if(events & NX_NOTIFY_ON_START_TOUCH)	printf("Start touch\n");
//		if(events & NX_NOTIFY_ON_END_TOUCH)		printf("End touch\n");

	// Iterate through contact points
	NxContactStreamIterator i(pair.stream);
	//user can call getNumPairs() here
	while(i.goNextPair())
	{
		//user can also call getShape() and getNumPatches() here
		while(i.goNextPatch())
		{
			//user can also call getPatchNormal() and getNumPoints() here
			const NxVec3& contactNormal = i.getPatchNormal();
			while(i.goNextPoint())
			{
				//user can also call getShape() and getNumPatches() here
				const NxVec3& contactPoint = i.getPoint();

				NxVec3 normalForceVec = NxVec3(0,0,0);
				NxVec3 frictionForceVec = NxVec3(0,0,0);
				NxVec3 penetrationVec = NxVec3(0,0,0);
				NxVec3 contactArrowForceTip = NxVec3(0,0,0);
				NxVec3 contactArrowFrictionTip = NxVec3(0,0,0);
				NxVec3 contactArrowPenetrationTip = NxVec3(0,0,0);

				ContactPoint cp(contactPoint, NxVec3(1,0,0));
				cpArray.pushBack(cp);

				// Get the normal force vector
				normalForceVec = pair.sumNormalForce;
//					normalForceVec = pair.sumNormalForce * 0.05;
				if (normalForceVec.magnitude() > 0.01)
				{
					normalForceVec.normalize();
					NxVec3 contactArrowForceTip = contactPoint + normalForceVec*3;
					ContactLine cl(contactPoint, contactArrowForceTip, NxVec3(0,0,1));
					clArray.pushBack(cl);
				}

				// Get the friction force vector
				frictionForceVec = pair.sumFrictionForce;
//					frictionForceVec = pair.sumFrictionForce* 0.05;
				if (frictionForceVec.magnitude() > 0.01)
				{
					frictionForceVec.normalize();
				    contactArrowFrictionTip = contactPoint + frictionForceVec*3;
				}

				// Get the penetration vector
				penetrationVec = - contactNormal * i.getSeparation();
//					penetrationVec = - contactNormal * i.getSeparation() * 20.0;
				if (penetrationVec.magnitude() > 0.01)
				{
					penetrationVec.normalize();
				    contactArrowPenetrationTip = contactPoint + penetrationVec*3;
				}
			}
		}
	}//*/
}


DLL_API void gTriggerReport::onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status)
{
	if (status & NX_TRIGGER_ON_ENTER)
	{
		// A body just entered the trigger area
		//gNbTouchedBodies++;
	}
	if (status & NX_TRIGGER_ON_LEAVE)
	{
		// A body just left the trigger area
		//gNbTouchedBodies--;
	}
	//NX_ASSERT(gNbTouchedBodies>=0);

	// Mark actors in the trigger area to apply forcefield forces to them
	NxActor* triggerActor = &triggerShape.getActor();
	if (((ActorUserData*)(triggerActor->userData))->flags & UD_IS_TRIGGER)
	{
        if (status & NX_TRIGGER_ON_ENTER)
        {
            NxActor* otherActor = &otherShape.getActor();
            ((ActorUserData*)(otherActor->userData))->flags |= NX_TRIGGER_ON_ENTER;
            ((ActorUserData*)(otherActor->userData))->triggerID = ((ActorUserData*)(triggerActor->userData))->id;
        }
        else if (status & NX_TRIGGER_ON_LEAVE)
        {
            NxActor* otherActor = &otherShape.getActor();
            ((ActorUserData*)(otherActor->userData))->flags |= NX_TRIGGER_ON_LEAVE;
            ((ActorUserData*)(otherActor->userData))->triggerID = ((ActorUserData*)(triggerActor->userData))->id;
        }
        else if (status & NX_TRIGGER_ON_STAY)
        {
            NxActor* otherActor = &otherShape.getActor();
            ((ActorUserData*)(otherActor->userData))->flags |= NX_TRIGGER_ON_STAY;
            ((ActorUserData*)(otherActor->userData))->triggerID = ((ActorUserData*)(triggerActor->userData))->id;
        }


	}
}


DLL_API bool gShapeReport::onEvent(NxU32 nbShapes, NxShape** shapes)
{
	while (nbShapes--)
	{
		NxActor* actor = &shapes[nbShapes]->getActor();
		((ActorUserData *)(actor->userData))->flags |= UD_PASSES_INTERSECTION_TEST;
	}
	return true;
}

DLL_API bool gRaycastReport::onHit(const NxRaycastHit& hit)
{
	//int userData = (int)hit.shape->getActor().userData;
	//userData |= 1;   // Mark as hit
	//hit.shape->getActor().userData = (void*)userData;
	const NxVec3& worldImpact = hit.worldImpact;
	return true;
}		