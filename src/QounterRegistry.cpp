#include "QounterRegistry.hpp"

using namespace QountersMinus;

const std::vector<QounterRegistry::EventHandlerSignature> QounterRegistry::eventHandlerSignatures = {
    {QounterRegistry::Event::NoteCut, "OnNoteCut", 2},
    {QounterRegistry::Event::NoteMiss, "OnNoteMiss", 1},
    {QounterRegistry::Event::ScoreUpdated, "OnScoreUpdated", 1},
    {QounterRegistry::Event::MaxScoreUpdated, "OnMaxScoreUpdated", 1},
    {QounterRegistry::Event::SwingRatingFinished, "OnSwingRatingFinished", 3},
};
std::unordered_map<std::pair<std::string, std::string>, QounterRegistry::RegistryEntry, pair_hash> QounterRegistry::registry;
std::vector<std::pair<std::string, std::string>> QounterRegistry::registryInsertionOrder;

void _DeactivateChildren(UnityEngine::GameObject* gameObject) {
    auto parent = gameObject->get_transform();
    for (int i = 0; i < parent->get_childCount(); i++) {
        parent->GetChild(i)->get_gameObject()->SetActive(false);
    }
}

void _DeactivateChildren(std::string gameObjectName) {
    _DeactivateChildren(UnityEngine::GameObject::Find(StringW(gameObjectName)));
}


void logScales(UnityEngine::GameObject* go, std::string path) {
    auto name = path + "/" + to_utf8(csstrtostr(go->get_name()));
    auto scale = go->get_transform()->get_localScale();
    LOG_DEBUG(name + ": %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);

    if (go->get_transform()->get_parent()) {
        logScales(go->get_transform()->get_parent()->get_gameObject(), name);
    }

    // for (int i = 0; i < go->get_transform()->get_childCount(); i++) {
    //     logScales(go->get_transform()->GetChild(i)->get_gameObject(), name);
    // }
}

void QountersMinus::QounterRegistry::Initialize() {
    for (auto&& def : registry) def.second.instance = nullptr;
    if (!Qounter::Enabled) return;
    if (UnityEngine::Object::FindObjectOfType<PlayerDataModel*>()->dyn__playerData()->get_playerSpecificSettings()->get_noTextsAndHuds()) return;
    if (UnityEngine::Object::FindObjectOfType<PlayerDataModel*>()->dyn__playerData()->get_gameplayModifiers()->get_zenMode()) return;
    auto AdvancedHUD = UnityEngine::Object::FindObjectOfType<PlayerDataModel*>()->dyn__playerData()->get_playerSpecificSettings()->get_advancedHud();
    if (!AdvancedHUD) return;

    auto comboPanel = UnityEngine::GameObject::Find(StringW("ComboPanel"));
    auto multiplierCanvas = UnityEngine::GameObject::Find(StringW("MultiplierCanvas"));

    if (Qounter::DisableIn90Degree) {
        // hacky way of getting BeatmapCharacteristic
        auto refs = comboPanel->AddComponent<QountersMinus::InjectedComponents*>();
        if (refs->beatmapCharacteristic->get_containsRotationEvents()) {
            UnityEngine::Object::Destroy(refs);
            return;
        }
    }

    if (Qounter::HideCombo) _DeactivateChildren(comboPanel);
    if (Qounter::HideMultiplier) {
        multiplierCanvas->GetComponent<UnityEngine::Animator*>()->set_enabled(false);
        _DeactivateChildren(multiplierCanvas);
    }
    if (Qounter::UprightInMultiplayer && UnityEngine::Object::FindObjectOfType<GlobalNamespace::MultiplayerLocalActivePlayerFacade*>()) {
        UnityEngine::Transform* transforms[4] = {
            comboPanel->get_transform(),
            multiplierCanvas->get_transform(),
            UnityEngine::GameObject::Find(StringW("ScoreCanvas"))->get_transform(),
            UnityEngine::GameObject::Find(StringW("SongProgressCanvas"))->get_transform()
        };
        for (auto transform : transforms) {
            transform->set_eulerAngles(UnityEngine::Vector3::get_zero());
            auto position = transform->get_localPosition();
            transform->set_localPosition(UnityEngine::Vector3(position.x, position.y + 1.8f, position.z + 4.0f));
        }
    }

    for (auto key : registryInsertionOrder) {
        auto&& def = registry[key];
        if (key.first == "QountersMinus" && key.second == "Qounter") continue; // there's got to be a better way
        auto enabled = *(bool*)def.staticFieldRefs["Enabled"];
        auto position = *(int*)def.staticFieldRefs["Position"];
        auto distance = *(float*)def.staticFieldRefs["Distance"];
        if (!enabled) continue;
        LOG_DEBUG("Initialize " + key.first + "::" + key.second);
        auto systemType = il2cpp_utils::GetSystemType(key.first, key.second);
        def.instance = QountersMinus::Qounter::Initialize(systemType, static_cast<QountersMinus::QounterPosition>(position), distance);
    }

     if (Qounter::ItalicText) {
        auto qounters = UnityEngine::Resources::FindObjectsOfTypeAll<Qounter*>();
        for (auto const& i : qounters) {
            auto texts = qounters[i].GetComponentsInChildren<TMPro::TextMeshProUGUI*>();
            for (auto const& j : texts) {
                texts[j].set_fontStyle(TMPro::FontStyles::Italic);
            }
        }
    }
}
