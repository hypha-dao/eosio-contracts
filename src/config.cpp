#include <hyphadao.hpp>
#include <document_graph/util.hpp>
#include <document_graph/content_group.hpp>
#include <document_graph/document.hpp>

#include <algorithm>

//using namespace hyphaspace;
using namespace hypha;

void hyphadao::createroot (const string &notes)
{
	require_auth (get_self());

	Document rootDoc(get_self(), get_self(), Content(common::ROOT_NODE, get_self()));
	rootDoc.emplace();

	//Create the settings document as well and add an edge to it
	ContentGroups settingCgs{{
		Content(CONTENT_GROUP_LABEL, common::SETTINGS),
		Content(common::ROOT_NODE, readableHash(rootDoc.hash))
	}};

	Document settingsDoc(get_self(), get_self(), std::move(settingCgs));
	settingsDoc.emplace();

	Edge rootToSettings(get_self(), get_self(), rootDoc.hash, settingsDoc.hash, common::SETTINGS_EDGE);
	rootToSettings.emplace();
}

checksum256 hyphadao::get_root (const name &contract)
{
	ContentGroups cgs = Document::rollup(Content(common::ROOT_NODE, contract));
    return Document::hashContents(cgs);
}

checksum256 hyphadao::get_root ()
{
	return get_root(get_self());
}

Document hyphadao::getSettingsDocument()
{
	auto root = get_root();
	
	auto edges = m_documentGraph.getEdgesFromOrFail(root, common::SETTINGS_EDGE);
	
	check(edges.size() == 1, "There should only exists only 1 settings edge from root node");

	return Document(get_self(), edges[0].to_node);
}

void hyphadao::setconfigatt(const string& key, const hyphadao::flexvalue1& value)
{
	require_auth(get_self());

	config_table config_s(get_self(), get_self().value);
	Config c = config_s.get_or_create(get_self(), Config());

	if (std::holds_alternative<name>(value))
	{
		c.names[key] = std::get<name>(value);
	}
	else if (std::holds_alternative<string>(value))
	{
		c.strings[key] = std::get<string>(value);
	}
	else if (std::holds_alternative<asset>(value))
	{
		c.assets[key] = std::get<asset>(value);
	}
	else if (std::holds_alternative<time_point>(value))
	{
		c.time_points[key] = std::get<time_point>(value);
	}
	else if (std::holds_alternative<uint64_t>(value))
	{
		c.ints[key] = std::get<uint64_t>(value);
	}
	c.time_points["updated_date"] = current_time_point();
 	config_s.set(c, get_self());
}

void hyphadao::setsetting(const string &key, const flexvalue& value)
{
	require_auth(get_self());

	auto document = getSettingsDocument();

	auto oldHash = document.hash;
	
	auto settingContent = Content(key, value);

	auto updateDateContent = Content(common::UPDATED_DATE, current_time_point());

	//Might want to return by & instead of const &
	auto contentGroups = document.content_groups;

	auto& settingsGroup = contentGroups[0];

	ContentWrapper::insertOrReplace(settingsGroup, settingContent);
	ContentWrapper::insertOrReplace(settingsGroup, updateDateContent);

	//We could to change ContentWrapper to store a regulare reference instead of a constant
	//Maybe also a ref to Document so we can rehashContents on data changes

	m_documentGraph.updateDocument(get_self(), oldHash, std::move(contentGroups));
}

void hyphadao::remsetting(const string &key)
{
	require_auth(get_self());

	auto document = getSettingsDocument();

	auto oldHash = document.hash;

	auto contentGroups = document.content_groups;

	auto& settingsGroup = contentGroups[0];

	auto isKey = [&key](auto &c) 
	{
		return c.label == key;
	};

	//First let's check if key exists
	auto contentItr = std::find_if(settingsGroup.begin(), settingsGroup.end(), isKey);

	if (contentItr != settingsGroup.end())
	{	
		settingsGroup.erase(contentItr);
		
		auto updateDateContent = Content(common::UPDATED_DATE, current_time_point());

		ContentWrapper::insertOrReplace(settingsGroup, updateDateContent);

		m_documentGraph.updateDocument(get_self(), oldHash, std::move(contentGroups));
	}
	//Should we assert if setting doesn't exits ?
	check(false, "The specified setting doesn't exits: " + key);
}

// void hyphadao::setalert (const name &level, const string &content)
// {
// 	// inline actions seem happiest when affixing types
// 	string alert_level = string {"alert_level"};
// 	string alert_content = string {"alert_content"};
// 	hyphadao::flexvalue1 fv_level = level;
// 	hyphadao::flexvalue1 fv_content = content;

// 	action(
// 		permission_level{get_self(), name("active")},
// 		get_self(), name("setconfigatt"),
// 		std::make_tuple(alert_level, fv_level))
// 	.send();
	
// 	action(
// 		permission_level{get_self(), name("active")},
// 		get_self(), name("setconfigatt"),
// 		std::make_tuple(alert_content, fv_content))
// 	.send();
// }

// void hyphadao::remalert (const string &notes)
// {
// 	// inline actions seem happiest when affixing types
// 	string alert_level = string {"alert_level"};
// 	string alert_content = string {"alert_content"};
	
// 	action(
// 		permission_level{get_self(), name("active")},
// 		get_self(), name("remconfigatt"),
// 		std::make_tuple(alert_level))
// 	.send();

// 	action(
// 		permission_level{get_self(), name("active")},
// 		get_self(), name("remconfigatt"),
// 		std::make_tuple(alert_content))
// 	.send();
// }


void hyphadao::updversion(const string &component, const string &version)
{
	setsetting(component, version);
}

void hyphadao::setlastballt(const name &last_ballot_id)
{
	setsetting(common::LAST_BALLOT_ID, last_ballot_id);
}

void hyphadao::togglepause()
{
	// int64_t new_state = !getSettingOrDefault<int64_t>(common::PAUSED);

	// setsetting(common::PAUSED, new_state);
}
