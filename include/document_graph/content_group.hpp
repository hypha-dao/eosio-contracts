#pragma once
#include <document_graph/content.hpp>

using std::string;

namespace hypha
{

    using ContentGroup = std::vector<Content>;
    using ContentGroups = std::vector<ContentGroup>;

    static const std::string CONTENT_GROUP_LABEL = std::string("content_group_label");

    class ContentGroupWrapper
    {

    public:
        static Content::FlexValue getValue(const ContentGroups &contentGroups,
                                           const std::string &groupLabel,
                                           const std::string &contentLabel);

        static Content::FlexValue getValue(const ContentGroup &contentGroup, const string &contentLabel);

        static ContentGroup getGroup(const ContentGroups &contentGroups, const string &groupLabel);

        static Content getContent(const ContentGroups &contentGroups, const std::string &groupLabel,
                                  const std::string &contentLabel);

        // not exactly sure how to do a template
        // template <typename T>
        // static T get(const ContentGroups &contentGroups,
        //              const std::string &groupLabel,
        //              const std::string &contentLabel);

        static eosio::asset getAsset(const ContentGroups &contentGroups,
                               const std::string &groupLabel,
                               const std::string &contentLabel);

        static std::string getString(const ContentGroups &contentGroups,
                        const std::string &groupLabel,
                        const std::string &contentLabel);

        static eosio::name getName(const ContentGroups &contentGroups,
                        const std::string &groupLabel,
                        const std::string &contentLabel);

        static std::int64_t getInt(const ContentGroups &contentGroups,
                        const std::string &groupLabel,
                        const std::string &contentLabel);

        // ContentGroupWrapper(ContentGroup& cg);
        // ~ContentGroupWrapper();

        // std::pair<int64_t, Content*> get (const std::string &label);
        // Content* getOrFail(const std::string &label, const std::string &error);

        // void add(Content item);
        // bool exists(const std::string &label);

        // ContentGroup& getItems();
        // const std::string toString();

        // static (stateless) parsers and finders
        // used to find data within ContentGroups
        // static std::pair<int64_t, Content *> get(ContentGroups contentGroups, const std::string &label);

        // static Content::FlexValue getValueOrFail(const ContentGroups &contentGroups,
        //                                          const string &contentGroupLabel,
        //                                          const string &contentLabel);

        // static ContentGroupWrapper* getContentGroupOrFail(const string &label);

        // static std::string getString(const ContentGroups &contentGroups,
        //                              const std::string &groupLabel,
        //                              const std::string &contentLabel);

        // static std::uint64_t getUint64(const ContentGroups &contentGroups,
        //                                const std::string &groupLabel,
        //                                const std::string &contentLabel);

        // static eosio::asset getAsset(const ContentGroups &contentGroups,
        //                              const std::string &groupLabel,
        //                              const std::string &contentLabel);

        // static eosio::name getName(const ContentGroups &contentGroups,
        //                            const std::string &groupLabel,
        //                            const std::string &contentLabel);

        // private:
        //     ContentGroup& m_contentGroup;
    };

} // namespace hypha