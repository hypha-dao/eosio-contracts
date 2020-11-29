#include <hyphadao.hpp>

using namespace hyphaspace;

void hyphadao::apply(const name &applicant,
					 const string &content)
{

	check(!is_paused(), "Contract is paused for maintenance. Please try again later.");
	require_auth(applicant);

	member_table m_t(get_self(), get_self().value);
	auto m_itr = m_t.find(applicant.value);
	check(m_itr == m_t.end(), "Applicant is already a member: " + applicant.to_string());

	applicant_table a_t(get_self(), get_self().value);
	auto a_itr = a_t.find(applicant.value);

	if (a_itr != a_t.end())
	{
		a_t.modify(a_itr, get_self(), [&](auto &a) {
			a.content = content;
			a.updated_date = current_time_point();
		});
	}
	else
	{
		a_t.emplace(get_self(), [&](auto &a) {
			a.applicant = applicant;
			a.content = content;
		});
	}

	map<string, hyphadao::flexvalue1> event_data;
	event_data["New Applicant"] = applicant;
	event_data["Content"] = content;
	event (name("medium"), event_data);
}

void hyphadao::enroll(const name &enroller,
					  const name &applicant,
					  const string &content)
{
	check(!is_paused(), "Contract is paused for maintenance. Please try again later.");

	// this action is linked to the dao.hypha@enrollers permission
	applicant_table a_t(get_self(), get_self().value);
	auto a_itr = a_t.find(applicant.value);
	check(a_itr != a_t.end(), "Applicant not found: " + applicant.to_string());

	asset one_hvoice = asset{100, common::S_HVOICE};
	string memo{"Welcome to Hypha DAO!"};
	action(
		permission_level{get_self(), name("active")},
		getSettingOrFail<name>(common::TELOS_DECIDE_CONTRACT), name("mint"),
		make_tuple(applicant, one_hvoice, memo))
		.send();

	action(
		permission_level{get_self(), name("active")},
		name("eosio"), name("buyram"),
		make_tuple(get_self(), applicant, common::RAM_ALLOWANCE))
		.send();

	// Should we also send 1 HYPHA?  I think so, so I'll put it for now, but comment it out
	// asset one_hypha = asset { 1, common::S_HYPHA };
	// bank.makepayment (-1, applicant, one_hypha, memo, common::NO_ASSIGNMENT);

	member_table m_t(get_self(), get_self().value);
	auto m_itr = m_t.find(applicant.value);
	check(m_itr == m_t.end(), "Account is already a member: " + applicant.to_string());
	m_t.emplace(get_self(), [&](auto &m) {
		m.member = applicant;
	});

	// update the graph
	checksum256 root_hash = get_root();
	document_graph::document member_doc = get_member_doc(enroller, applicant);
	_document_graph.create_edge(root_hash, member_doc.hash, common::MEMBER);
	_document_graph.create_edge(member_doc.hash, root_hash, common::MEMBER_OF);

	// broadcast event
	map<string, hyphadao::flexvalue1> event_data;
	event_data["New Member"] = applicant;
	event_data["Enroller"] = enroller;
	event_data["Content"] = content;
	event_data["Purchased RAM"] = common::RAM_ALLOWANCE;
	event (name("high"), event_data);

	a_t.erase(a_itr);
}

document_graph::document hyphadao::get_member_doc (const name& member)
{
	auto ctnt = _document_graph.new_content("member", member);
	document_graph::content_group cg; 
	cg.push_back(ctnt);
	vector<document_graph::content_group> cgs;
	cgs.push_back(cg);
	return _document_graph.get_document(_document_graph.hash_document(cgs));
}

document_graph::document hyphadao::get_member_doc (const name& creator, const name& member)
{
	return _document_graph.get_or_create (creator, _document_graph.new_content(common::MEMBER_STRING, member));
}

void hyphadao::verify_membership (const name& member) 
{
	// create hash to represent this member account
	std::vector<document_graph::content_group> member_cgs;
	document_graph::content_group member_cg = document_graph::content_group{};
	member_cg.push_back(_document_graph.new_content("member", member));
	member_cgs.push_back(member_cg);
	checksum256 member_hash = _document_graph.hash_document(member_cgs);

	// check to see if this member has a document saved
	document_graph::document member_doc = _document_graph.get_document(member_hash);

	checksum256 root_hash = get_root();
	// verify that the member_hash is a MEMBER of the root_hash
	edge_table e_t (get_self(), get_self().value);
    auto itr = e_t.find (_document_graph.edge_id (root_hash, member_hash, common::MEMBER));

	check (itr != e_t.end(), "account: " + member.to_string() + " is not a member of " + _document_graph.readable_hash(root_hash));
}

void hyphadao::makememdocs (const string &notes)
{
	checksum256 root = get_root();

	member_table m_t(get_self(), get_self().value);
	auto m_itr = m_t.begin();
	while (m_itr != m_t.end()) {
		auto member = get_member_doc (get_self(), m_itr->member); 
		// the root node holds the member as on a member EDGE
		_document_graph.create_edge (root, member.hash, common::MEMBER);

		// the member is a member of EDGE the root
		_document_graph.create_edge (member.hash, root, common::MEMBER_OF);
   
		m_itr++;
	}
};

void hyphadao::remapply(const name &applicant)
{
	require_auth(get_self());
	applicant_table a_t(get_self(), get_self().value);
	auto a_itr = a_t.find(applicant.value);
	a_t.erase(a_itr);
}


void hyphadao::addmember(const name &member)
{
	require_auth(get_self());
	member_table m_t(get_self(), get_self().value);
	auto m_itr = m_t.find(member.value);
	check(m_itr == m_t.end(), "Account is already a member: " + member.to_string());
	m_t.emplace(get_self(), [&](auto &m) {
		m.member = member;
	});
}

void hyphadao::removemember(const name &member)
{
	require_auth(get_self());
	member_table m_t(get_self(), get_self().value);
	auto m_itr = m_t.find(member.value);
	check(m_itr != m_t.end(), "Account is not a member: " + member.to_string());
	m_t.erase(m_itr);
}
