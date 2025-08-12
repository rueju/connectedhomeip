/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "ContentLauncherManager.h"
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/util/config.h>

#include <list>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::DataModel;
using namespace chip::app::Clusters::ContentLauncher;

ContentLauncherManager::ContentLauncherManager(std::list<std::string> acceptHeaderList, uint32_t supportedStreamingProtocols)
{
    mAcceptHeaderList            = acceptHeaderList;
    mSupportedStreamingProtocols = supportedStreamingProtocols;

    // Add dummy content for test cases
    ContentEntry entry1;
    entry1.mName = "TV Show Example";
    ParameterType parameter1;
    parameter1.type  = ParameterEnum::kActor;
    parameter1.value = chip::CharSpan::fromCharString("Gaby sHoffman");
    ParameterType parameter2;
    parameter2.type  = ParameterEnum::kChannel;
    parameter2.value = chip::CharSpan::fromCharString("PBS");
    ParameterType parameter3;
    parameter3.type  = ParameterEnum::kCharacter;
    parameter3.value = chip::CharSpan::fromCharString("Snow White");
    ParameterType parameter4;
    parameter4.type  = ParameterEnum::kDirector;
    parameter4.value = chip::CharSpan::fromCharString("Spike Lee");
    ParameterType parameter5;
    parameter5.type  = ParameterEnum::kFranchise;
    parameter5.value = chip::CharSpan::fromCharString("Star Wars");
    ParameterType parameter6;
    parameter6.type  = ParameterEnum::kGenre;
    parameter6.value = chip::CharSpan::fromCharString("Horror");
    ParameterType parameter7;
    parameter7.type  = ParameterEnum::kPopularity;
    parameter7.value = chip::CharSpan::fromCharString("Popularity");
    ParameterType parameter8;
    parameter8.type  = ParameterEnum::kProvider;
    parameter8.value = chip::CharSpan::fromCharString("Netflix");
    entry1.mSearchFields.push_back(parameter1);
    entry1.mSearchFields.push_back(parameter2);
    entry1.mSearchFields.push_back(parameter3);
    entry1.mSearchFields.push_back(parameter4);
    entry1.mSearchFields.push_back(parameter5);
    entry1.mSearchFields.push_back(parameter6);
    entry1.mSearchFields.push_back(parameter7);
    entry1.mSearchFields.push_back(parameter8);
    mContentList.push_back(entry1);

    ContentEntry entry2;
    entry2.mName = "Sports Example";
    ParameterType parameter21;
    parameter21.type  = ParameterEnum::kEvent;
    parameter21.value = chip::CharSpan::fromCharString("Football games");
    ParameterType parameter22;
    parameter22.type  = ParameterEnum::kLeague;
    parameter22.value = chip::CharSpan::fromCharString("NCAA");
    ParameterType parameter23;
    parameter23.type  = ParameterEnum::kSport;
    parameter23.value = chip::CharSpan::fromCharString("football");
    ParameterType parameter24;
    parameter24.type  = ParameterEnum::kSportsTeam;
    parameter24.value = chip::CharSpan::fromCharString("Arsenel");
    ParameterType parameter25;
    parameter25.type  = ParameterEnum::kType;
    parameter25.value = chip::CharSpan::fromCharString("TVSeries");
    entry2.mSearchFields.push_back(parameter21);
    entry2.mSearchFields.push_back(parameter22);
    entry2.mSearchFields.push_back(parameter23);
    entry2.mSearchFields.push_back(parameter24);
    entry2.mSearchFields.push_back(parameter25);
    mContentList.push_back(entry2);
}

void ContentLauncherManager::HandleLaunchContent(CommandResponseHelper<LaunchResponseType> & helper,
                                                 const DecodableList<ParameterType> & parameterList, bool autoplay,
                                                 const CharSpan & data,
                                                 const chip::Optional<PlaybackPreferencesType> playbackPreferences,
                                                 bool useCurrentContext)
{
    ChipLogProgress(Zcl, "ContentLauncherManager::HandleLaunchContent");
    std::string dataString(data.data(), data.size());

    ChipLogProgress(Zcl, "ContentLauncherManager::HandleLaunchContent TEST CASE autoplay=%d data=%s ", (autoplay ? 1 : 0),
                    dataString.c_str());

    // TODO: Add playbackPreferences as part of search query
    // TODO: Add useCurrentContext as part of search query

    bool foundMatch = false;
    for (auto const & contentEntry : this->mContentList)
    {
        auto iter = parameterList.begin();
        while (iter.Next())
        {
            auto & parameterType = iter.GetValue();
            for (auto const & parameter : contentEntry.mSearchFields)
            {
                if (parameter.type == parameterType.type)
                {
                    std::string val1(parameter.value.data(), parameter.value.size());
                    std::string val2(parameterType.value.data(), parameterType.value.size());
                    if (strcmp(val1.c_str(), val2.c_str()) == 0)
                    {
                        ChipLogProgress(Zcl, " TEST CASE found match=%s type=%d", contentEntry.mName.c_str(),
                                        static_cast<uint16_t>(parameter.type));
                        foundMatch = true;
                    }
                }
            }
        }
    }

    if (!foundMatch)
    {
        ChipLogProgress(Zcl, " TEST CASE did not find a match");
    }

    LaunchResponseType response;
    // TODO: Insert code here
    response.data   = chip::MakeOptional(CharSpan::fromCharString("exampleData"));
    response.status = ContentLauncher::StatusEnum::kSuccess;
    helper.Success(response);
}

void LaunchYouTube(const char * url)
{
    char buf[512];
    snprintf(buf, sizeof(buf), "{\"type\":\"youtube\",\"url\":\"%s\"}", url);

    // Write to /tmp/matter.json using fopen/fwrite
    FILE *out = fopen("/tmp/matter.json", "w");
    if (out)
    {
        fwrite(buf, 1, strlen(buf), out);
        fclose(out);
    }
    else
    {
        ChipLogError(Controller, "Failed to write /tmp/matter.json");
    }

}

void LaunchOmega(const char * url)
{
    char buf[512];
    snprintf(buf, sizeof(buf), "{\"type\":\"omega\",\"url\":\"%s\"}", url);

    // Write to /tmp/matter.json using fopen/fwrite
    FILE *out = fopen("/tmp/matter.json", "w");
    if (out)
    {
        fwrite(buf, 1, strlen(buf), out);
        fclose(out);
    }
    else
    {
        ChipLogError(Controller, "Failed to write /tmp/matter.json");
    }

}

void SetUnsupportedType(const char * url)
{
    char buf[512];
    snprintf(buf, sizeof(buf), "{\"type\":\"unsupported\",\"url\":\"%s\"}", url);

    // Write to /tmp/matter.json using fopen/fwrite
    FILE *out = fopen("/tmp/matter.json", "w");
    if (out)
    {
        fwrite(buf, 1, strlen(buf), out);
        fclose(out);
    }
    else
    {
        ChipLogError(Controller, "Failed to write /tmp/matter.json");
    }

}

// Function to convert string to lowercase (helper function)
void to_lowercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Function to check if string ends with a specific suffix (case-insensitive)
int ends_with_ignore_case(const char* str, const char* suffix) {
    int str_len = strlen(str);
    int suffix_len = strlen(suffix);

    if (suffix_len > str_len) {
        return 0;
    }

    // Create lowercase copies for comparison
    char str_lower[str_len + 1];
    char suffix_lower[suffix_len + 1];

    strcpy(str_lower, str + (str_len - suffix_len));
    strcpy(suffix_lower, suffix);

    to_lowercase(str_lower);
    to_lowercase(suffix_lower);

    return strcmp(str_lower, suffix_lower) == 0;
}

// Function to check if string contains substring (case-insensitive)
int contains_ignore_case(const char* str, const char* substr) {
    int str_len = strlen(str);
    int substr_len = strlen(substr);

    char str_lower[str_len + 1];
    char substr_lower[substr_len + 1];

    strcpy(str_lower, str);
    strcpy(substr_lower, substr);

    to_lowercase(str_lower);
    to_lowercase(substr_lower);

    return strstr(str_lower, substr_lower) != NULL;
}

int check_url_type(const char* url) {
    if (url == NULL || strlen(url) == 0) {
        return 0; // Invalid input
    }

    // Check for YouTube URLs
    if (strncmp(url, "https://www.youtube.com/watch?v=", 32) == 0 ||
        strncmp(url, "https://youtube.com/watch?v=", 28) == 0 ||
        strncmp(url, "https://www.youtu.be/", 21) == 0 ||
        strncmp(url, "https://youtu.be/", 17) == 0 ||
        strncmp(url, "http://www.youtube.com/watch?v=", 31) == 0 ||
        strncmp(url, "http://youtube.com/watch?v=", 27) == 0 ||
        strncmp(url, "http://www.youtu.be/", 20) == 0 ||
        strncmp(url, "http://youtu.be/", 16) == 0) {
        return 1; // YouTube video
    }

    // Check for manifest URLs
    // First check file extensions
    if (ends_with_ignore_case(url, ".mpd") ||
        ends_with_ignore_case(url, ".m3u8") ||
        ends_with_ignore_case(url, ".m3u")) {
        return 2; // Manifest URL
    }

    // Check for manifest URLs with query parameters
    if (contains_ignore_case(url, ".mpd?") ||
        contains_ignore_case(url, ".m3u8?") ||
        contains_ignore_case(url, ".m3u?")) {
        return 2; // Manifest URL with parameters
    }

    // Check for common manifest URL patterns in the path
    if (contains_ignore_case(url, "/manifest.mpd") ||
        contains_ignore_case(url, "/playlist.m3u8") ||
        contains_ignore_case(url, "/master.m3u8") ||
        contains_ignore_case(url, "/index.m3u8")) {
        return 2; // Common manifest naming patterns
    }

    return 0; // Neither YouTube nor manifest
}

char* convert_to_youtube_tv_url(const char* original_url) {
    const char* video_id_start = NULL;
    int video_id_length = 0;

    // Check for standard YouTube URL format
    const char* youtube_prefix = "https://www.youtube.com/watch?v=";
    if (strncmp(original_url, youtube_prefix, strlen(youtube_prefix)) == 0) {
        // Extract video ID from standard format
        video_id_start = strstr(original_url, "v=");
        if (video_id_start == NULL) {
            return NULL;
        }
        video_id_start += 2; // Skip "v="

        // Find the end of video ID (stop at & or end of string)
        const char* video_id_end = strchr(video_id_start, '&');
        if (video_id_end != NULL) {
            video_id_length = video_id_end - video_id_start;
        } else {
            video_id_length = strlen(video_id_start);
        }
    }
    // Check for shortened youtu.be URL format
    else {
        const char* youtu_be_prefix = "https://www.youtu.be/";
        const char* youtu_be_prefix_alt = "https://youtu.be/";

        if (strncmp(original_url, youtu_be_prefix, strlen(youtu_be_prefix)) == 0) {
            video_id_start = original_url + strlen(youtu_be_prefix);
        } else if (strncmp(original_url, youtu_be_prefix_alt, strlen(youtu_be_prefix_alt)) == 0) {
            video_id_start = original_url + strlen(youtu_be_prefix_alt);
        } else {
            return NULL; // Not a recognized YouTube URL format
        }

        // Find the end of video ID (stop at ? or end of string)
        const char* video_id_end = strchr(video_id_start, '?');
        if (video_id_end != NULL) {
            video_id_length = video_id_end - video_id_start;
        } else {
            video_id_length = strlen(video_id_start);
        }
    }

    // Validate video ID length (YouTube video IDs are typically 11 characters)
    if (video_id_length <= 0 || video_id_length > 20) {
        return NULL;
    }

    // Calculate the length needed for the new URL
    const char* tv_prefix = "https://www.youtube.com/tv/#/watch?v=";
    int new_url_length = strlen(tv_prefix) + video_id_length + 1;

    // Allocate memory for the new URL
    char* new_url = (char *)malloc(new_url_length);
    if (new_url == NULL) {
        return NULL; // Memory allocation failed
    }

    // Build the new URL
    strcpy(new_url, tv_prefix);
    strncat(new_url, video_id_start, video_id_length);

    ChipLogProgress(Zcl, "New YouTube URL: %s", new_url);

    return new_url;
}

void LaunchUrl(const char * url)
{
   int type = check_url_type(url);
   char *new_url = NULL;
   ChipLogProgress(Zcl, "URL type : %d", type);
   if (1 == type)
   {
      new_url = convert_to_youtube_tv_url(url);
      LaunchYouTube(new_url);
      free(new_url);
   }
   else if (2 == type)
   {
      ChipLogProgress(Zcl, "MPD URL");
      LaunchOmega(url);
   }
   else
   {
      ChipLogProgress(Zcl, "Unsupported type");
      SetUnsupportedType(url);
   }
}

void ContentLauncherManager::HandleLaunchUrl(CommandResponseHelper<LaunchResponseType> & helper, const CharSpan & contentUrl,
                                             const CharSpan & displayString, const BrandingInformationType & brandingInformation)
{
    ChipLogProgress(Zcl, "ContentLauncherManager::HandleLaunchUrl");

    std::string contentUrlString(contentUrl.data(), contentUrl.size());
    std::string displayStringString(displayString.data(), displayString.size());
    std::string providerNameString(brandingInformation.providerName.data(), brandingInformation.providerName.size());

    ChipLogProgress(
        Zcl, "ContentLauncherManager::HandleLaunchUrl TEST CASE ContentURL=%s DisplayString=%s BrandingInformation.ProviderName=%s",
        contentUrlString.c_str(), displayStringString.c_str(), providerNameString.c_str());

    // TODO: Insert code here
    LaunchUrl(contentUrlString.c_str());
    LaunchResponseType response;
    response.data   = chip::MakeOptional(CharSpan::fromCharString("exampleData"));
    response.status = ContentLauncher::StatusEnum::kSuccess;

    // Handle test cases
    if (contentUrlString == "https://badurl")
    {
        response.status = ContentLauncher::StatusEnum::kURLNotAvailable;
    }
    else if (contentUrlString == "https://csa-iot.org/badauth")
    {
        response.status = ContentLauncher::StatusEnum::kAuthFailed;
    }

    helper.Success(response);
}

CHIP_ERROR ContentLauncherManager::HandleGetAcceptHeaderList(AttributeValueEncoder & aEncoder)
{
    ChipLogProgress(Zcl, "ContentLauncherManager::HandleGetAcceptHeaderList");
    return aEncoder.EncodeList([this](const auto & encoder) -> CHIP_ERROR {
        for (std::string & entry : mAcceptHeaderList)
        {
            CharSpan data = CharSpan::fromCharString(entry.c_str());
            ReturnErrorOnFailure(encoder.Encode(data));
        }
        return CHIP_NO_ERROR;
    });
}

uint32_t ContentLauncherManager::HandleGetSupportedStreamingProtocols()
{
    ChipLogProgress(Zcl, "ContentLauncherManager::HandleGetSupportedStreamingProtocols");
    return mSupportedStreamingProtocols;
}

uint32_t ContentLauncherManager::GetFeatureMap(chip::EndpointId endpoint)
{
    if (endpoint >= MATTER_DM_CONTENT_LAUNCHER_CLUSTER_SERVER_ENDPOINT_COUNT)
    {
        return kEndpointFeatureMap;
    }

    uint32_t featureMap = 0;
    Attributes::FeatureMap::Get(endpoint, &featureMap);
    return featureMap;
}

uint16_t ContentLauncherManager::GetClusterRevision(chip::EndpointId endpoint)
{
    if (endpoint >= MATTER_DM_CONTENT_LAUNCHER_CLUSTER_SERVER_ENDPOINT_COUNT)
    {
        return kClusterRevision;
    }

    uint16_t clusterRevision = 0;
    bool success =
        (Attributes::ClusterRevision::Get(endpoint, &clusterRevision) == chip::Protocols::InteractionModel::Status::Success);
    if (!success)
    {
        ChipLogError(Zcl, "ContentLauncherManager::GetClusterRevision error reading cluster revision");
    }
    return clusterRevision;
}
