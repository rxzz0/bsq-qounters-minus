#include "InjectedComponents.hpp"

using namespace QountersMinus;
using namespace GlobalNamespace;

DEFINE_TYPE(QountersMinus, InjectedComponents);

int InjectedComponents::numInstances = 0;
SaberManager* InjectedComponents::saberManager;
CoreGameHUDController* InjectedComponents::coreGameHUDController;
RelativeScoreAndImmediateRankCounter* InjectedComponents::relativeScoreAndImmediateRankCounter;
ScoreController* InjectedComponents::scoreController;
PlayerData* InjectedComponents::playerData;
IDifficultyBeatmap* InjectedComponents::difficultyBeatmap;
BeatmapCharacteristicSO* InjectedComponents::beatmapCharacteristic;

void QountersMinus::InjectedComponents::Awake() {
    if (numInstances++ == 0) {
        saberManager = UnityEngine::Object::FindObjectOfType<SaberManager*>();
        coreGameHUDController = UnityEngine::Object::FindObjectOfType<CoreGameHUDController*>();
        relativeScoreAndImmediateRankCounter = UnityEngine::Object::FindObjectOfType<RelativeScoreAndImmediateRankCounter*>();
        scoreController = UnityEngine::Object::FindObjectOfType<ScoreController*>();
        playerData = UnityEngine::Object::FindObjectOfType<PlayerDataModel*>()->get_playerData();

        auto gameplayCoreInstallers = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::GameplayCoreInstaller*>();
        GlobalNamespace::GameplayCoreInstaller* gameplayCoreInstaller;
        for (int i = 0; i < gameplayCoreInstallers; i++) {
            if (gameplayCoreInstallers[i]->get_isActiveAndEnabled() && gameplayCoreInstallers[i]->dyn__sceneSetupData() != nullptr) {
                gameplayCoreInstaller = gameplayCoreInstallers[i];
                break;
            }
        }
        if (!gameplayCoreInstaller) gameplayCoreInstaller = gameplayCoreInstallers[0];
        difficultyBeatmap = gameplayCoreInstaller->dyn__sceneSetupData()->dyn_difficultyBeatmap();
        beatmapCharacteristic = difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic();
    }
}

void QountersMinus::InjectedComponents::OnDestroy() {
    if (--numInstances == 0) {
        saberManager = nullptr;
        coreGameHUDController = nullptr;
        relativeScoreAndImmediateRankCounter = nullptr;
        scoreController = nullptr;
        playerData = nullptr;
        difficultyBeatmap = nullptr;
    }
}
