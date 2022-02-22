#include "util/note_count.hpp"

using namespace GlobalNamespace;

int GetNoteCount() {
    static auto noteDataType = il2cpp_utils::GetSystemType("", "NoteData"); 
    int noteCount = 0;
    auto bocc = UnityEngine::Object::FindObjectOfType<BeatmapObjectCallbackController*>();
    auto songTime = bocc->dyn__initData()->dyn_spawningStartTime();
    auto beatmapLinesData = reinterpret_cast<BeatmapData*>(bocc->dyn__initData()->dyn_beatmapData())->dyn__beatmapLinesData();
    getLogger().info("beatmapLinesData: %lu", beatmapLinesData.Length());
    for (int i = 0; i < beatmapLinesData.Length(); i++) {
        auto beatmapLineData = beatmapLinesData[i];
        auto beatmapObjectsData = beatmapLineData->dyn__beatmapObjectsData()->dyn__items();
        for (int j = 0; j < beatmapObjectsData.Length(); j++) {
            auto beatmapObjectData = beatmapObjectsData[j];
                 if (!beatmapObjectData) continue;
                 if (!noteDataType->IsAssignableFrom(beatmapObjectData->GetType())) continue;
                    auto note = reinterpret_cast<NoteData*>(beatmapObjectData);
                    auto noteColorType = note->get_colorType();
                    auto NoteTime = note->get_time();
                         if (noteColorType != GlobalNamespace::ColorType::None && NoteTime > songTime) {
                            noteCount++;
                }
            }
        }
        return noteCount;
    }