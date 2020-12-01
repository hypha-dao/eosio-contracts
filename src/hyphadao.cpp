#include <memory>
#include <hyphadao.hpp>

#include <document_graph/content_group.hpp>
#include <proposals/proposal_factory.hpp>
#include <proposals/proposal.hpp>

namespace hypha
{

	hyphadao::hyphadao(name self, name code, datastream<const char *> ds) : contract(self, code, ds) {}

	hyphadao::~hyphadao() {}

	void hyphadao::propose(const name &proposer,
						   const name &proposal_type,
						   ContentGroups &content_groups)
	{
		check(!is_paused(), "Contract is paused for maintenance. Please try again later.");

		eosio::name self = get_self();
		std::unique_ptr<Proposal> proposal = std::unique_ptr<Proposal>(ProposalFactory::Factory(self, proposal_type));
		proposal->propose(proposer, content_groups);
		// delete proposal;
	}

	void hyphadao::closedocprop(const checksum256 &proposal_hash)
	{
		check(!is_paused(), "Contract is paused for maintenance. Please try again later.");

		Document docprop(get_self(), proposal_hash);
		name proposal_type = ContentWrapper::getContent(docprop.content_groups, common::SYSTEM, common::TYPE).getAs<eosio::name>();

		Proposal *proposal = ProposalFactory::Factory(get_self(), proposal_type);
		proposal->close(docprop);
		// delete proposal;
	}

	// void hyphadao::payassign(const checksum256 &assignment_hash, const uint64_t &period_id)
	// {
		// check(!is_paused(), "Contract is paused for maintenance. Please try again later.");

		// Document assignment = _document_graph.get_document(assignment_hash);
		// ContentGroup assignment_details = _document_graph.get_content_group(assignment, common::DETAILS, true);

		// name assignee = std::get<name>(m_dao._document_graph.get_content(details, common::ASSIGNEE, true));

		// // assignee must still be a DHO member
		// verify_membership(assignee);
		// require_auth(assignee);

		// // get the role
		// Document role = _document_graph.get_document(std::get<checksum256>(_document_graph.get_content(details, common::ROLE_STRING, true)));
		// string badge_title = std::get<string>(document_graph::get_content(badge, common::DETAILS, common::TITLE, true));

		// // Check that the assignment has not been paid for this period yet
		// asspay_table asspay_t(get_self(), get_self().value);
		// auto period_index = asspay_t.get_index<name("byperiod")>();
		// auto per_itr = period_index.find(period_id);
		// while (per_itr->period_id == period_id && per_itr != period_index.end())
		// {
		// 	check(per_itr->assignment_id != assignment_id, "Assignment ID has already been paid for this period. Period ID: " +
		// 													   std::to_string(period_id) + "; Assignment ID: " + std::to_string(assignment_id));
		// 	per_itr++;
		// }

		// // Check that the period has elapsed
		// period_table period_t(get_self(), get_self().value);
		// auto p_itr = period_t.find(period_id);
		// check(p_itr != period_t.end(), "Cannot make payment. Period ID not found: " + std::to_string(period_id));
		// check(p_itr->end_date.sec_since_epoch() < current_block_time().to_time_point().sec_since_epoch(),
		// 	  "Cannot make payment. Period ID " + std::to_string(period_id) + " has not closed yet.");

		// // Check that the creation date of the assignment is before the end of the period
		// check(a_itr->created_date.sec_since_epoch() < p_itr->end_date.sec_since_epoch(),
		// 	  "Cannot make payment to assignment. Assignment was not approved before this period.");

		// // Check that pay period is between (inclusive) the start and end period of the role and the assignment
		// check(a_itr->ints.at("start_period") <= period_id && a_itr->ints.at("end_period") >= period_id, "For assignment, period ID must be between " +
		// 																									std::to_string(a_itr->ints.at("start_period")) + " and " + std::to_string(a_itr->ints.at("end_period")) + " (inclusive). You tried: " + std::to_string(period_id));

		// // We've disabled this check that confirms the period being claimed falls within a role's guidelines
		// // likely this will replaced with budgeting anyways
		// // check(r_itr->ints.at("start_period") <= period_id && r_itr->ints.at("end_period") >= period_id, "For role, period ID must be between " +
		// // 																									std::to_string(r_itr->ints.at("start_period")) + " and " + std::to_string(r_itr->ints.at("end_period")) + " (inclusive). You tried: " + std::to_string(period_id));

		// float first_phase_ratio_calc = 1; // pro-rate based on elapsed % of the first phase

		// // Pro-rate the payment if the assignment was created during the period being claimed
		// if (a_itr->created_date.sec_since_epoch() > p_itr->start_date.sec_since_epoch())
		// {
		// 	auto elapsed_sec = p_itr->end_date.sec_since_epoch() - a_itr->created_date.sec_since_epoch();
		// 	auto period_sec = p_itr->end_date.sec_since_epoch() - p_itr->start_date.sec_since_epoch();
		// 	debugx("elapsed sec : " + std::to_string(elapsed_sec) + "  period_sec " + std::to_string(period_sec));
		// 	first_phase_ratio_calc = (float)elapsed_sec / (float)period_sec;
		// }

		// // If there is an explicit ESCROW SEEDS salary amount, support sending it; else it should be calculated
		// asset deferred_seeds_payment;
		// if (a_itr->assets.find("seeds_escrow_salary_per_phase") != a_itr->assets.end())
		// {
		// 	deferred_seeds_payment = adjust_asset(a_itr->assets.at("seeds_escrow_salary_per_phase"), first_phase_ratio_calc);
		// }
		// else if (a_itr->assets.find("usd_salary_value_per_phase") != a_itr->assets.end())
		// {
		// 	// Dynamically calculate the SEEDS amount based on the price at the end of the period being claimed
		// 	deferred_seeds_payment = adjust_asset(get_seeds_amount(a_itr->assets.at("usd_salary_value_per_phase"),
		// 														   p_itr->end_date,
		// 														   get_float(a_itr->ints, "time_share_x100"),
		// 														   get_float(a_itr->ints, "deferred_perc_x100")),
		// 										  first_phase_ratio_calc);
		// }
		// else
		// {
		// 	deferred_seeds_payment = asset{0, common::S_SEEDS};
		// }

		// // If there is an explicity INSTANT SEEDS amount, support sending it, else it should be zero
		// asset instant_seeds_payment;
		// if (a_itr->assets.find("seeds_instant_salary_per_phase") != a_itr->assets.end())
		// {
		// 	instant_seeds_payment = adjust_asset(a_itr->assets.at("seeds_instant_salary_per_phase"), first_phase_ratio_calc);
		// }
		// else
		// {
		// 	instant_seeds_payment = asset{0, common::S_SEEDS};
		// }

		// // These values are calculated when the assignment is proposed, so simply pro-rate them if/as needed
		// asset husd_payment = adjust_asset(a_itr->assets.at("husd_salary_per_phase"), first_phase_ratio_calc);
		// asset voice_payment = adjust_asset(a_itr->assets.at("hvoice_salary_per_phase"), first_phase_ratio_calc);
		// asset hypha_payment = adjust_asset(a_itr->assets.at("hypha_salary_per_phase"), first_phase_ratio_calc);

		// // Record the payment as a whole (for the assignment/period combo); NOTE: bank also records individual payments per token
		// asspay_t.emplace(get_self(), [&](auto &a) {
		// 	a.ass_payment_id = asspay_t.available_primary_key();
		// 	a.assignment_id = assignment_id;
		// 	a.recipient = a_itr->names.at("assigned_account"),
		// 	a.period_id = period_id;
		// 	a.payment_date = current_block_time().to_time_point();
		// 	a.payments.push_back(hypha_payment);
		// 	a.payments.push_back(deferred_seeds_payment);
		// 	a.payments.push_back(instant_seeds_payment);
		// 	a.payments.push_back(voice_payment);
		// 	a.payments.push_back(husd_payment);
		// });

		// string memo = "Payment for role " + std::to_string(a_itr->ints.at("role_id")) + "; Assignment ID: " + std::to_string(assignment_id) + "; Period ID: " + std::to_string(period_id);

		// debugx("Processing payments: HYPHA: " + hypha_payment.to_string() +
		// 	   ", DEFERRED: " + deferred_seeds_payment.to_string() +
		// 	   ", INSTANT: " + instant_seeds_payment.to_string() +
		// 	   ", HVOICE: " + voice_payment.to_string() +
		// 	   ", HUSD: " + husd_payment.to_string());

		// // creating a single struct improves performance for table queries here
		// asset_batch ab{};
		// ab.d_seeds = deferred_seeds_payment;
		// ab.hypha = hypha_payment;
		// ab.seeds = instant_seeds_payment;
		// ab.voice = voice_payment;
		// ab.husd = husd_payment;

		// ab = apply_badge_coefficients(period_id, a_itr->names.at("assigned_account"), ab);

		// // Make all payments to the assigned account; NOTE: the makepayment function simply returns if the amount is zero
		// // The bank knows how to send various payments/tokens
		// make_payment(period_id, a_itr->names.at("assigned_account"), ab.hypha, memo, assignment_id, 1);
		// make_payment(period_id, a_itr->names.at("assigned_account"), ab.d_seeds, memo, assignment_id, 0);
		// make_payment(period_id, a_itr->names.at("assigned_account"), ab.seeds, memo, assignment_id, 1);
		// make_payment(period_id, a_itr->names.at("assigned_account"), ab.voice, memo, assignment_id, 1);
		// make_payment(period_id, a_itr->names.at("assigned_account"), ab.husd, memo, assignment_id, 1);
	// }
} // namespace hypha