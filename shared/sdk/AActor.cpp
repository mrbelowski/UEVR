#include <vector>
#include "UObjectArray.hpp"

#include "AActor.hpp"

namespace sdk {
UClass* AActor::static_class() {
    return sdk::find_uobject<UClass>(L"Class /Script/Engine.Actor");
}

bool AActor::set_actor_location(const glm::vec3& location, bool sweep, bool teleport) {
    static const auto func = static_class()->find_function(L"K2_SetActorLocation");
    static const auto fhitresult = sdk::find_uobject<UScriptStruct>(L"ScriptStruct /Script/Engine.HitResult");
    static const auto fvector = sdk::find_uobject<UScriptStruct>(L"ScriptStruct /Script/CoreUObject.Vector");

    const auto is_ue5 = fvector->get_struct_size() == sizeof(glm::vec<3, double>);

    // Need to dynamically allocate the params because of unknown FHitResult size
    std::vector<uint8_t> params{};

    // add a vec3
    if (!is_ue5) {
        params.insert(params.end(), (uint8_t*)&location, (uint8_t*)&location + sizeof(glm::vec3));
    } else {
        glm::vec<3, double> loc = location;
        params.insert(params.end(), (uint8_t*)&loc, (uint8_t*)&loc + sizeof(glm::vec<3, double>));
    }
    // add a bool
    params.insert(params.end(), (uint8_t*)&sweep, (uint8_t*)&sweep + sizeof(bool));
    // align    
    params.insert(params.end(), 3, 0);
    // add a FHitResult
    params.insert(params.end(), fhitresult->get_struct_size(), 0);
    // add a bool
    params.insert(params.end(), (uint8_t*)&teleport, (uint8_t*)&teleport + sizeof(bool));
    // add a bool
    bool ret = false;
    void* ret_ptr = &ret;
    params.insert(params.end(), (uint8_t*)&ret_ptr, (uint8_t*)&ret_ptr + sizeof(void*));

    this->process_event(func, params.data());

    return ret;
}

glm::vec3 AActor::get_actor_location() {
    static const auto func = static_class()->find_function(L"K2_GetActorLocation");
    static const auto fvector = sdk::find_uobject<UScriptStruct>(L"ScriptStruct /Script/CoreUObject.Vector");

    const auto is_ue5 = fvector->get_struct_size() == sizeof(glm::vec<3, double>);

    std::vector<uint8_t> params{};

    // add a vec3
    if (!is_ue5) {
        params.insert(params.end(), sizeof(glm::vec3), 0);
    } else {
        params.insert(params.end(), sizeof(glm::vec<3, double>), 0);
    }

    this->process_event(func, params.data());

    if (!is_ue5) {
        return *(glm::vec3*)params.data();
    }
        
    return *(glm::vec<3, double>*)params.data();
};
}