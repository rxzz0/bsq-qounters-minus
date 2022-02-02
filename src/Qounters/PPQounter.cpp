#include "Qounters/PPQounter.hpp"

DEFINE_TYPE(QountersMinus::Qounters,PPQounter);

bool QountersMinus::Qounters::PPQounter::Enabled = false;
int QountersMinus::Qounters::PPQounter::Position = static_cast<int>(QountersMinus::QounterPosition::AboveHighway);
float QountersMinus::Qounters::PPQounter::Distance = 1.0f;
bool QountersMinus::Qounters::PPQounter::HideWhenUnranked = true;

static const float MULT_DISAPPEARINGARROWS_ORIGINAL = 0.07f;
static const float MULT_DISAPPEARINGARROWS_SCORESABER = 0.02f;
static const float MULT_GHOSTNOTES_ORIGINAL = 0.11f;
static const float MULT_GHOSTNOTES_SCORESABER = 0.04f;
static const float MULT_FASTERSONG_ORIGINAL = 0.08f;
static const float MULT_FASTERSONG_SCORESABER = 0.08f;

void QountersMinus::Qounters::PPQounter::Register() {
    QounterRegistry::Register<PPQounter>("PP", "PP Qounter", "PPConfig", true);
    QounterRegistry::RegisterConfig<PPQounter>({
        .ptr = &HideWhenUnranked,
        .field = "HideWhenUnranked",
        .displayName = "Hide When Unranked",
        .helpText = "Whether the Qounter should be shown at all in unranked songs.",
        .type = QounterRegistry::ConfigType::Bool,
    });
}

GlobalNamespace::GameplayModifiers* RemovePositiveModifiers(GlobalNamespace::GameplayModifiers* modifiers) {
    return GlobalNamespace::GameplayModifiers::New_ctor(
        modifiers->get_demoNoFail(),
        modifiers->get_demoNoObstacles(),
        modifiers->get_energyType(),
        modifiers->get_noFailOn0Energy(),
        modifiers->get_instaFail(),
        modifiers->get_failOnSaberClash(),
        modifiers->get_enabledObstacleType(),
        modifiers->get_noBombs(),
        modifiers->get_fastNotes(),
        modifiers->get_strictAngles(),
        false, // disappearingArrows
        (modifiers->get_songSpeed() == GlobalNamespace::GameplayModifiers::SongSpeed::Faster)
            ? (GlobalNamespace::GameplayModifiers::SongSpeed)GlobalNamespace::GameplayModifiers::SongSpeed::Normal
            : modifiers->get_songSpeed(),
        modifiers->get_noArrows(),
        false, // ghostNotes
        false, // proMode
        modifiers->get_zenMode(),
        false // smallCubes
    );
}

float CalculateMultiplier(std::string songID, GlobalNamespace::ScoreController* scoreController) {
    auto modifiers = QountersMinus::PP::SongAllowsPositiveModifiers(songID) ?
        scoreController->dyn__gameplayModifiers() : RemovePositiveModifiers(scoreController->dyn__gameplayModifiers());

    auto multiplier = scoreController->dyn__gameplayModifiersModel()->GetTotalMultiplier(
        scoreController->dyn__gameplayModifierParams(), // TODO: call RemovePositiveModifiers here
        scoreController->dyn__gameEnergyCounter()->get_energy()
    );
    // auto multiplier = scoreController->gameplayModifiersModel->GetTotalMultiplier(modifiers, scoreController->gameEnergyCounter->energy);

    // ScoreSaber weights these multipliers differently
    if (modifiers->get_disappearingArrows())
        multiplier += (MULT_DISAPPEARINGARROWS_SCORESABER - MULT_DISAPPEARINGARROWS_ORIGINAL);
    if (modifiers->get_ghostNotes())
        multiplier += (MULT_GHOSTNOTES_SCORESABER - MULT_GHOSTNOTES_ORIGINAL);
    if (modifiers->get_songSpeed() == GlobalNamespace::GameplayModifiers::SongSpeed::Faster)
        multiplier += (MULT_FASTERSONG_SCORESABER - MULT_FASTERSONG_ORIGINAL);

    return multiplier;
}

void QountersMinus::Qounters::PPQounter::Start() {
    auto songID = GetSongID(refs->difficultyBeatmap);
    auto ppData = QountersMinus::PP::BeatmapMaxPP(songID.hash, songID.difficulty);
    isRanked = ppData.has_value();
    if (HideWhenUnranked && !isRanked) return;

    if (isRanked) maxPP = ppData.has_value() ? ppData.value() : 0.0f;
    multiplier = CalculateMultiplier(songID.hash, refs->scoreController);
    CreateBasicText("PP: --");
    if (basicText) {
    basicText->get_rectTransform()->set_anchoredPosition(UnityEngine::Vector2(0.0f, 0.0f));
    }
}

void QountersMinus::Qounters::PPQounter::OnScoreUpdated(int modifiedScore) {
    if (HideWhenUnranked && !isRanked) return;
    if (maxPP > 0.0f) {
        auto accuracy = refs->relativeScoreAndImmediateRankCounter->get_relativeScore() * multiplier;
        auto currentPP = QountersMinus::PP::CalculatePP(maxPP, accuracy);
        if (basicText) {
        basicText->set_text(StringW("PP: " + FormatNumber(currentPP, 2)));
        }
    }
}
