#include <eosio/eosio.hpp>
#include <document_graph/content_group.hpp>
#include <document_graph/content.hpp>

namespace hypha
{
    ContentGroup ContentGroupWrapper::getGroup(const ContentGroups &contentGroups, const std::string &groupLabel)
    {
        for (std::size_t i = 0; i < contentGroups.size(); ++i)
        {
            for (const Content &content : contentGroups[i])
            {
                if (content.label == CONTENT_GROUP_LABEL)
                {
                    eosio::check(std::holds_alternative<std::string>(content.value), "fatal error: " + CONTENT_GROUP_LABEL + " must be a string");
                    if (std::get<std::string>(content.value) == groupLabel)
                    {
                        return contentGroups[i];
                    }
                }
            }
        }
        eosio::check(false, "no content with label found: " + groupLabel);
        return ContentGroup{};
    }

    Content::FlexValue ContentGroupWrapper::getValue(const ContentGroup &contentGroup, const std::string &contentLabel)
    {
        for (std::size_t i = 0; i < contentGroup.size(); ++i)
        {
            if (contentGroup[i].label == contentLabel)
            {
                return contentGroup[i].value;
            }
        }
        return Content::FlexValue{};
    }

    Content::FlexValue ContentGroupWrapper::getValue(const ContentGroups &contentGroups, const std::string &groupLabel,
                                                     const std::string &contentLabel)
    {
        return getValue(getGroup(contentGroups, groupLabel), contentLabel);
    }

    Content ContentGroupWrapper::getContent(const ContentGroups &contentGroups, const std::string &groupLabel,
                                            const std::string &contentLabel)
    {
        ContentGroup contentGroup = getGroup(contentGroups, groupLabel);
        for (std::size_t i = 0; i < contentGroup.size(); ++i)
        {
            if (contentGroup[i].label == contentLabel)
            {
                return contentGroup[i];
            }
        }
        return Content{};
    }

    // template <typename T>
    // T ContentGroupWrapper::get(const ContentGroups &contentGroups,
    //                            const std::string &groupLabel,
    //                            const std::string &contentLabel)
    // {
    //     Content::FlexValue flex = getValue(contentGroups, groupLabel, contentLabel);
    //     eosio::check(std::holds_alternative<T>(flex),
    //                  "Content group: " + contentLabel + ", item: " + contentLabel +
    //                      " is not of expected type");
    //     return std::get<T>(flex);
    // }

    eosio::asset ContentGroupWrapper::getAsset(const ContentGroups &contentGroups,
                                               const std::string &groupLabel,
                                               const std::string &contentLabel)
    {
        Content::FlexValue flex = getValue(contentGroups, groupLabel, contentLabel);
        eosio::check(std::holds_alternative<eosio::asset>(flex),
                     "Content group: " + contentLabel + ", item: " + contentLabel +
                         " is not of expected type");
        return std::get<eosio::asset>(flex);
    }

    std::string ContentGroupWrapper::getString(const ContentGroups &contentGroups,
                                               const std::string &groupLabel,
                                               const std::string &contentLabel)
    {
        Content::FlexValue flex = getValue(contentGroups, groupLabel, contentLabel);
        eosio::check(std::holds_alternative<std::string>(flex),
                     "Content group: " + contentLabel + ", item: " + contentLabel +
                         " is not of expected type");
        return std::get<std::string>(flex);
    }

    std::int64_t ContentGroupWrapper::getInt(const ContentGroups &contentGroups,
                                             const std::string &groupLabel,
                                             const std::string &contentLabel)
    {
        Content::FlexValue flex = getValue(contentGroups, groupLabel, contentLabel);
        eosio::check(std::holds_alternative<std::int64_t>(flex),
                     "Content group: " + contentLabel + ", item: " + contentLabel +
                         " is not of expected type");
        return std::get<std::int64_t>(flex);
    }

    eosio::name ContentGroupWrapper::getName(const ContentGroups &contentGroups,
                                             const std::string &groupLabel,
                                             const std::string &contentLabel)
    {
        Content::FlexValue flex = getValue(contentGroups, groupLabel, contentLabel);
        eosio::check(std::holds_alternative<eosio::name>(flex),
                     "Content group: " + contentLabel + ", item: " + contentLabel +
                         " is not of expected type");
        return std::get<eosio::name>(flex);
    }

    // ContentGroupWrapper::ContentGroupWrapper(ContentGroup& cg) : m_contentGroup{cg} {}
    // ContentGroupWrapper::~ContentGroupWrapper() {}

    // std::pair<int64_t, Content *> ContentGroupWrapper::get(const std::string &label)
    // {
    //     for (std::size_t i = 0; i < m_contentGroup.size(); ++i)
    //     {
    //         if (m_contentGroup[i].label == label)
    //         {
    //             return {(int64_t)i, &m_contentGroup[i]};
    //         }
    //     }

    //     return {-1, nullptr};
    // }

    // Content* ContentGroupWrapper::getOrFail(const std::string &label, const std::string &error)
    // {
    //     auto [idx, item] = get(label);
    //     if (idx == -1)
    //     {
    //         eosio::check(false, error);
    //     }
    //     return item;
    // }

    // std::pair<int64_t, ContentGroup *> ContentGroupWrapper::getConntGroup(const std::string &label)
    // {
    //     for (std::size_t i = 0; i < content_groups.size(); ++i)
    //     {
    //         for (const Content &content : content_groups[i])
    //         {
    //             if (content.label == CONTENT_GROUP_LABEL)
    //             {
    //                 eosio::check(std::holds_alternative<std::string>(content.value), "fatal error: " + CONTENT_GROUP_LABEL + " must be a string");
    //                 if (std::get<std::string>(content.value) == label)
    //                 {
    //                     ContentGroupWrapper cgw (content_groups[i]);
    //                     return {(int64_t)i, &cgw};
    //                 }
    //             }
    //         }
    //     }
    //     return {-1, nullptr};
    // }

    // ContentGroupWrapper* ContentGroupWrapper::getContentGroupOrFail(const std::string &label, const std::string &error)
    // {
    //     auto [idx, contentGroup] = getContentGroup(label);
    //     if (idx == -1)
    //     {
    //         eosio::check(false, error);
    //     }
    //     return contentGroup;
    // }

} // namespace hypha