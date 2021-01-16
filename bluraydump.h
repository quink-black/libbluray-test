//
// Created by Zhao Zhili on 2021/1/16.
//

#ifndef BLURAY_BLURAYDUMP_H
#define BLURAY_BLURAYDUMP_H

#include <string>
#include <sstream>
#include <map>

#include <libbluray/bluray.h>
#include <libbluray/meta_data.h>

class BlurayDump {
public:
    ~BlurayDump() {
        release();
    }

    bool init(const char *path) {
        bluray_ = bd_open(path, nullptr);
        return bluray_ != nullptr;
    }

    void release() {
        if (bluray_ != nullptr) {
            bd_close(bluray_);
            bluray_ = nullptr;
        }
    }

    std::map<std::string, std::string> dumpDiscInfo() {
        std::map<std::string, std::string> info;
        if (bluray_ == nullptr) {
            std::cerr << "invalid state\n";
            return info;
        }

        auto disc_info = bd_get_disc_info(bluray_);
        if (disc_info == nullptr) {
            std::cerr << "bd_get_disc_info return null\n";
            return info;
        }

        info["disc_name"] = toStr(disc_info->disc_name);
        info["udf_volume_id"] = toStr(disc_info->udf_volume_id);

        info["num_titles"] = std::to_string(disc_info->num_titles);
        std::ostringstream ss;
        for (int i = 0; i < disc_info->num_titles; i++) {
            ss << "\n\ttitle " << (i + 1) << ": " << toStr(disc_info->titles[i]->name);
        }
        info["titles"] = ss.str();

        info["video_format"] = getVideoFormatName(disc_info->video_format);
        auto frame_rate = getFrameRate(disc_info->frame_rate);
        info["frame_rate"] = std::to_string(frame_rate.first) + "/" + std::to_string(frame_rate.second);
        info["3d"] = disc_info->content_exist_3D ? "true" : "false";
        info["provider_data"] = std::string((char*)disc_info->provider_data, sizeof(disc_info->provider_data));
        return info;
    }

    std::map<std::string, std::string> dumpMetadata() {
        std::map<std::string, std::string> info;
        if (bluray_ == nullptr) {
            std::cerr << "invalid state\n";
            return info;
        }

        auto meta = bd_get_meta(bluray_);
        if (meta == nullptr) {
            std::cerr << "bd_get_meta return null\n";
            return info;
        }

        info["language"] = std::string(meta->language_code, sizeof(meta->language_code));
        info["filename"] = toStr(meta->filename);
        info["di_name"] = toStr(meta->di_name);
        info["di_alternative"] = toStr(meta->di_alternative);

        info["title_count"] = std::to_string(meta->toc_count);
        {
            std::ostringstream ss;
            for (uint32_t i = 0; i < meta->toc_count; i++) {
                ss << "\n\ttitle number " << meta->toc_entries[i].title_number << ", "
                   << "title name " << meta->toc_entries[i].title_name;
            }
            info["title_entries"] = ss.str();
        }

        info["thumb_count"] = std::to_string(meta->thumb_count);
        {
            std::ostringstream ss;
            for (unsigned i = 0; i < meta->thumb_count; i++) {
                auto thumb = meta->thumbnails[i];
                uint32_t w = (thumb.xres == UINT32_MAX) ? 0 : thumb.xres;
                uint32_t h = (thumb.yres == UINT32_MAX) ? 0 : thumb.yres;
                ss << "\n\tthumbnail path " << thumb.path << ", resolution "
                   << w << "x" << h;
            }
            info["thumbs"] = ss.str();
        }

        return info;
    }

private:
    static std::string toStr(const char *str) {
        return str != nullptr ? std::string(str) : std::string("");
    }

    static std::string getVideoFormatName(int format) {
        switch (format) {
            case BLURAY_VIDEO_FORMAT_480I:
                return "480I";
            case BLURAY_VIDEO_FORMAT_576I:
                return "576I";
            case BLURAY_VIDEO_FORMAT_480P:
                return "480P";
            case BLURAY_VIDEO_FORMAT_1080I:
                return "1080I";
            case BLURAY_VIDEO_FORMAT_720P:
                return "720P";
            case BLURAY_VIDEO_FORMAT_1080P:
                return "1080P";
            case BLURAY_VIDEO_FORMAT_576P:
                return "576P";
            case BLURAY_VIDEO_FORMAT_2160P:
                return "2160P";
            default:
                return "unknown";
        }
    }

    static std::pair<int, int> getFrameRate(int rate) {
        switch (rate) {
            case BLURAY_VIDEO_RATE_24000_1001:
                return std::make_pair(24000, 1001);
            case BLURAY_VIDEO_RATE_24:
                return std::make_pair(24, 1);
            case BLURAY_VIDEO_RATE_25:
                return std::make_pair(25, 1);
            case BLURAY_VIDEO_RATE_30000_1001:
                return std::make_pair(30000, 1001);
            case BLURAY_VIDEO_RATE_50:
                return std::make_pair(50, 1);
            case BLURAY_VIDEO_RATE_60000_1001:
                return std::make_pair(60000, 1001);
            default:
                return std::make_pair(-1, 1);
        }
    }

private:
    BLURAY *bluray_ = nullptr;
};

#endif //BLURAY_BLURAYDUMP_H