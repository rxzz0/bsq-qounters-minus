#include "util/note_count.hpp"

int GetNoteCount() {
    static auto noteDataType = il2cpp_utils::GetSystemType("", "NoteData");
    int noteCount = 0;
    auto bocc = UnityEngine::Object::FindObjectOfType<GlobalNamespace::BeatmapObjectCallbackController*>();
    auto songTime = bocc->dyn__initData()->dyn_spawningStartTime();
    auto beatmapLinesData = reinterpret_cast<GlobalNamespace::BeatmapData*>(bocc->dyn__initData()->dyn_beatmapData())->dyn__beatmapLinesData();
    for (int i = 0; i < beatmapLinesData; i++) {
        auto beatmapLineData = beatmapLinesData[i];
        auto beatmapObjectsData = beatmapLinesData;
        for (int j = 0; j < beatmapObjectsData; j++) {
            auto beatmapObjectData = beatmapObjectsData[j];
            if (!beatmapObjectData) continue;
            if (!noteDataType->IsAssignableFrom(beatmapObjectData->GetType())) continue;
            auto note = reinterpret_cast<GlobalNamespace::NoteData*>(beatmapObjectData);
            if (note->get_colorType() != GlobalNamespace::ColorType::None && note->get_time() > songTime) {
                noteCount++;
            }
        }
    }
    return noteCount;
}