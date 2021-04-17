// (C) 2021 Nicolaus Anderson
#ifndef __EXISTENCE_SIGNAL_OBJ_H__
#define __EXISTENCE_SIGNAL_OBJ_H__

#include <IReferenceCounted.h>

namespace irr
{
/*
	Sometimes objects contain data that is pointed to by pointers in external objects
	that are not aware of when the data container is destroyed.
	This flag can be used by the data container to signal that it has been destroyed and
	therefore all pointers to its data are now invalid.
	Both the data container and the objects containing the pointers to the data must
	reference count and point to this existence signal object.

	Usage:
	In your constructor, create an irrptr to contain this object for automatic reference counting.
	irr::tools::irrptr<ExistSignal> p;
	//... cstor
	ExistSignal* es = new ExistSignal(this);
	p.set(es);
	es->drop();
*/
struct ExistSignal
	: public IReferenceCounted
{
	//! This class must be inherited by the owner/creator object.
	struct Owner {
		operator(Owner*) {
			return (Owner*)this;
		}
	};

private:
	bool exists;
	Owner* owner;

public:
	ExistSignal( Owner* pOwner )
		: exists(true)
		, owner(pOwner)
	{}

	void TargetDestroyed( Owner* pNotifier ) {
		if ( owner == pNotifier ) {
			exists = false;
		}
	}

	bool TargetExists() { return exists; }
};

}

#endif /* __EXISTENCE_SIGNAL_OBJ_H__ */
