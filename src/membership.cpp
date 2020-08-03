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
}

void hyphadao::enroll(const name &enroller,
					  const name &applicant,
					  const string &content)
{

	check(!is_paused(), "Contract is paused for maintenance. Please try again later.");

	// this action is linked to the hyphadaomain@enrollers permission
	applicant_table a_t(get_self(), get_self().value);
	auto a_itr = a_t.find(applicant.value);
	check(a_itr != a_t.end(), "Applicant not found: " + applicant.to_string());

	config_table config_s(get_self(), get_self().value);
	Config c = config_s.get_or_create(get_self(), Config());

	asset one_hvoice = asset{100, common::S_HVOICE};
	string memo{"Welcome to Hypha DAO!"};
	action(
		permission_level{get_self(), "active"_n},
		c.names.at("telos_decide_contract"), name("mint"),
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

	a_t.erase(a_itr);
}

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