#include "SKSELink.h"

//Big shout out to https://github.com/mwilsnd for his project https://github.com/mwilsnd/SkyrimSE-SmoothCam which helped amazingly with the address lib stuff!

Undaunted::RelocAddr<_PlaceAtMe_Native> PlaceAtMerec(0x009951F0);

Undaunted::RelocPtr<PlayerCharacter*> thePlayer(0x02F26EF8);

Undaunted::RelocPtr <DataHandler*> dataHandler(0x01EBE428);

DataHandler* Undaunted::GetDataHandler() {
	return *dataHandler;
}

PlayerCharacter* Undaunted::GetPlayer()
{
	return *thePlayer;
}

TESObjectREFR* Undaunted::PlaceAtMe(VMClassRegistry* registry, int count, TESObjectREFR* ref, TESForm* spawnForm, int something, bool ForcePersist, bool InitiallyDisabled)
{
	return PlaceAtMerec(registry, count, ref, spawnForm, 1, ForcePersist, InitiallyDisabled);
}


static VersionDb db;

VersionDb Offsets::GetDB()
{
	return db;
}

bool Offsets::Initialize() {
	return db.Load();
}

#ifdef _DEBUG
void Offsets::DumpDatabaseTextFile() {
	if (!GetDB().Load(1, 5, 97, 0)) {
		//FatalError(L"Failed to load offset database.");
	}

	GetDB().Dump("offsets.txt");
}
#endif

constexpr uintptr_t Offsets::GetByVersionAddr(uintptr_t addr) {
	return addrMap.at(addr);
}

uintptr_t Offsets::GetVersionAddress(uintptr_t addr) {
	return GetOffset(addrMap.at(addr));
}

uintptr_t Offsets::GetOffset(uintptr_t id) {
	uintptr_t ret;
	GetDB().FindOffsetById(id, ret);
	return ret;
}