#include <hyphadao.hpp>

using namespace hyphaspace;

hyphadao::hyphadao(name self, name code, datastream<const char*> ds) : contract(self, code, ds) {}

hyphadao::~hyphadao() {}

void hyphadao::withdraw (const name &withdrawer, const uint64_t &assignment_id, const string& notes) 
{
	// check paused state
	check(!is_paused(), "Contract is paused for maintenance. Please try again later.");
	require_auth (withdrawer);

	// confirm that the object to be suspended exists
	object_table original_t (get_self(), name("assignment").value);
	auto original_itr = original_t.find(assignment_id);
	check(original_itr != original_t.end(), "Cannot withdraw, original does not exist. Assignment: " + 
		std::to_string(assignment_id) + " does not exist.");
	
	check (original_itr->names.at("assigned_account") == withdrawer, "Only the assigned account can withdraw from an assignment. You are: " + 
		withdrawer.to_string() + " but the assigned account is " + original_itr->names.at("assigned_account").to_string());

	original_t.modify (original_itr, get_self(), [&](auto &o) {
		o.ints["end_period"] = get_last_period_id();
		o.time_points["withdrawal_date"] = current_time_point();
		o.strings["withdrawal_notes"] = notes;
		o.updated_date = current_time_point();
	});
}

void hyphadao::propsuspend (const name& proposer, const name& scope, const uint64_t& id) 
{
	// check paused state
	check(!is_paused(), "Contract is paused for maintenance. Please try again later.");
	require_auth (proposer);

	// confirm that the object to be suspended exists
	object_table original_t (get_self(), scope.value);
	auto original_itr = original_t.find(id);
	check(original_itr != original_t.end(), "Cannot suspend, original does not exist. Scope: " + scope.to_string() + 
		"; Original Object ID: " + std::to_string(id) + " does not exist.");		

	map<string, name> temp_names {}; 
	map<string, uint64_t> temp_ints {}; 
	
	temp_names["original_scope"] = scope;		
	temp_ints["original_object_id"] = id;
	temp_names["owner"] = proposer;
	temp_names["type"] = name("suspend");
	temp_names["trx_action_name"] = name("suspend");

	// not used, just required for passing to new_proposal (non-const must be lvalues)
	map<string, string> not_used_strings {};
	map<string, asset> not_used_assets {};
	map<string, time_point> not_used_timepoints {};
	map<string, transaction> not_used_transactions {};

	new_proposal (proposer, temp_names, not_used_strings, not_used_assets, 
			not_used_timepoints, temp_ints, not_used_transactions );
}

void hyphadao::edit(const name &scope,
					const uint64_t &id,
					const map<string, name> names,
					map<string, string> strings,
					map<string, asset> assets,
					map<string, time_point> time_points,
					const map<string, uint64_t> ints,
					const map<string, float> floats,
					map<string, transaction> trxs) {

	// check paused state
	check(!is_paused(), "Contract is paused for maintenance. Please try again later.");
	check(scope != name("proposal"), "Proposals cannot be edited; it must be re-proposed.");

	// check to see if original object exists
	object_table original_t (get_self(), scope.value);
	auto original_itr = original_t.find(id);
	check(original_itr != original_t.end(), "Cannot edit, original does not exist. Scope: " + scope.to_string() + 
		"; Original Object ID: " + std::to_string(id) + " does not exist.");

	name owner = original_itr->names.at("owner");
	check(has_auth(owner) || has_auth(get_self()), "Authentication failed. Must have authority from owner: " +
													   owner.to_string() + "@active or " + get_self().to_string() + "@active.");

	if (scope == name("draft")) {
		merge(scope, id, names, strings, assets, time_points, ints, trxs);
	} else {
		// if the proposal was created by 'self' and NOT the owner, flag it as so
		map<string, uint64_t> temp_ints = ints; 
		if (!has_auth(owner)) {
			temp_ints["created_by_owner"] = 0;
		}

		map<string, name> temp_names = names; 
		
		temp_names["original_scope"] = scope;	
		temp_ints["original_object_id"] = id;
		temp_names["type"] = name("edit");
		temp_names["trx_action_name"] = name("mergeobject");

		new_proposal (owner, temp_names, strings, assets, time_points, temp_ints, trxs);
	}
}

void hyphadao::create(const name &scope,
					  map<string, name> names,
					  map<string, string> strings,
					  map<string, asset> assets,
					  map<string, time_point> time_points,
					  map<string, uint64_t> ints,
					  const map<string, float> floats,
					  map<string, transaction> trxs)
{
	// check paused state
	check(!is_paused(), "Contract is paused for maintenance. Please try again later.");

	// owner is currently required; TODO: promote to top level parameter?
	check (names.find("owner") != names.end(), "owner is a required entry for the names map");

	const name owner = names.at("owner");
	check(has_auth(owner) || has_auth(get_self()), "Authentication failed. Must have authority from owner: " +
													   owner.to_string() + "@active or " + get_self().to_string() + "@active.");

	// if the proposal was created by 'self' and NOT the owner, flag it as so
	map<string, uint64_t> temp_ints = ints; 
	if (!has_auth(owner)) {
		temp_ints["created_by_owner"] = 0;
	}

	qualify_owner(owner);

	if (scope == name("draft")) {
		new_object(owner, scope, names, strings, assets, time_points, temp_ints, trxs);
	} else {
		new_proposal (owner, names, strings, assets, time_points, temp_ints, trxs);
	}
}

void hyphadao::created(const name &creator, const checksum256 &hash)
{
	// only the contract can announce this
	require_auth(get_self());
}

void hyphadao::createdoc (const name& creator, const vector<document_graph::content_group> &content_groups) 
{
	_document_graph.create_document (creator, content_groups);
}

void hyphadao::exectrx(const uint64_t &proposal_id)
{
	require_auth(get_self());

	// proposal_table p_t (get_self(), get_self().value);
	// auto p_itr = p_t.find (proposal_id);
	// check (p_itr != p_t.end(), "Proposal ID: " + std::to_string(proposal_id) + " does not exist.");
	// check (p_itr->trxs.size() > 1, "There are not transactions to execute. Trx map size: " + std::to_string(p_itr->trxs.size()) + "; Proposal ID: " + std::to_string(proposal_id));

	// for (auto trx_itr = p_itr->trxs.begin(); trx_itr != p_itr->trxs.end(); ++trx_itr) {
	// 	// skip the transaction named "exec_on_approval" because that is the one currently executing
	// 	// debug ( "trx_itr->first	: " + trx_itr->first);
	// 	// debug ( "true/false		: " + std::to_string(trx_itr->first.compare("exec_on_approval") == 0));
	// 	if (trx_itr->first.compare("exec_on_approval") == 0) {
	// 		debug ("Executing transaction	: " + trx_itr->first);
	// 		trx_itr->second.send(current_block_time().to_time_point().sec_since_epoch(), get_self());
	// 	}
	// }
}

void hyphadao::newrole(const uint64_t &proposal_id)
{
	require_auth(get_self());
	vector<name> new_scopes = {name("role"), name("proparchive")};
	changescope(name("proposal"), proposal_id, new_scopes, true);
}

void hyphadao::suspend(const uint64_t &proposal_id)
{
	require_auth(get_self());

	object_table o_t_proposal(get_self(), name("proposal").value);
	auto o_itr = o_t_proposal.find(proposal_id);
	check(o_itr != o_t_proposal.end(), "Scope: " + name("proposal").to_string() + "; Object ID: " + std::to_string(proposal_id) + " does not exist.");

	// ACTUALLY change the end_period on the object
	object_table original_t (get_self(), o_itr->names.at("original_scope").value);
	auto original_itr = original_t.find(o_itr->ints.at("original_object_id"));
	check(original_itr != original_t.end(), "Scope: " + o_itr->names.at("original_scope").to_string() + 
		"; Original Object ID: " + std::to_string(o_itr->ints.at("original_object_id")) + " does not exist.");

	original_t.modify (original_itr, get_self(), [&](auto &o) {
		o.ints["end_period"] = get_last_period_id();
		o.time_points["suspension_date"] = current_time_point();
		o.updated_date = current_time_point();
	});

	// move the proposal to the archives
	vector<name> new_scopes = {name("edit"), name("proparchive")};
	changescope(name("proposal"), proposal_id, new_scopes, true);
}

void hyphadao::assign(const uint64_t &proposal_id)
{
	require_auth(get_self());

	object_table o_t_proposal(get_self(), name("proposal").value);
	auto o_itr = o_t_proposal.find(proposal_id);
	check(o_itr != o_t_proposal.end(), "Scope: " + name("proposal").to_string() + "; Object ID: " + std::to_string(proposal_id) + " does not exist.");

	// Ensure that the owner of this proposer doesn't already have this assignment
	// object_table o_t_assignment(get_self(), name("assignment").value);
	// auto sorted_by_owner = o_t_assignment.get_index<name("byowner")>();
	// auto a_itr = sorted_by_owner.find(o_itr->names.at("owner").value);

	// this prevents the same user from signing up for the same assignment, disabled since we are enforcing role capacity
	// while (a_itr != sorted_by_owner.end() && a_itr->names.at("owner") == o_itr->names.at("owner"))
	// {
	// 	check(!(a_itr->ints.at("role_id") == o_itr->ints.at("role_id") && a_itr->names.at("owner") == o_itr->names.at("owner")),
	// 		  "Proposal owner already has this role. Owner: " + o_itr->names.at("owner").to_string() + "; Role ID: " + std::to_string(o_itr->ints.at("role_id")));
	// 	a_itr++;
	// }

	// check_capacity(o_itr->ints.at("role_id"), o_itr->ints.at("time_share_x100"));

	vector<name> new_scopes = {name("assignment"), name("proparchive")};
	changescope(name("proposal"), proposal_id, new_scopes, true);
}


void hyphadao::mergeobject(const uint64_t &proposal_id) {
	require_auth(get_self());

	object_table o_t(get_self(), name("proposal").value);
	auto proposal_itr = o_t.find(proposal_id);
	check(proposal_itr != o_t.end(), "Scope: " + name("proposal").to_string() + "; Object ID: " + std::to_string(proposal_id) + " does not exist.");

	merge (	proposal_itr->names.at("original_scope"), 
			proposal_itr->ints.at("original_object_id"),  
			proposal_itr->names, 
			proposal_itr->strings, 
			proposal_itr->assets, 
			proposal_itr->time_points, 
			proposal_itr->ints, 
			proposal_itr->trxs);
	
	vector<name> new_scopes = {name("edit"), name("proparchive")};
	changescope(name("proposal"), proposal_id, new_scopes, true);
}

void hyphadao::makepayout(const uint64_t &proposal_id)
{
	require_auth(get_self());

	object_table o_t(get_self(), name("proposal").value);
	auto o_itr = o_t.find(proposal_id);
	check(o_itr != o_t.end(), "Scope: " + name("proposal").to_string() + "; Object ID: " + std::to_string(proposal_id) + " does not exist.");

	string memo{"One time payout for Hypha Contribution. Proposal ID: " + std::to_string(proposal_id)};
	make_payment(-1, o_itr->names.at("recipient"), o_itr->assets.at("hypha_amount"), memo, common::NO_ASSIGNMENT, 1);
	make_payment(-1, o_itr->names.at("recipient"), o_itr->assets.at("husd_amount"), memo, common::NO_ASSIGNMENT, 1);
	make_payment(-1, o_itr->names.at("recipient"), o_itr->assets.at("hvoice_amount"), memo, common::NO_ASSIGNMENT, 1);
	make_payment(-1, o_itr->names.at("recipient"), o_itr->assets.at("seeds_instant_amount"), memo, common::NO_ASSIGNMENT, 1);
	make_payment(-1, o_itr->names.at("recipient"), o_itr->assets.at("seeds_escrow_amount"), memo, common::NO_ASSIGNMENT, 0);

	vector<name> new_scopes = {name("payout"), name("proparchive")};
	changescope(name("proposal"), proposal_id, new_scopes, true);
}

bool hyphadao::did_pass (const name &ballot_id)
{
	config_table config_s(get_self(), get_self().value);
	Config c = config_s.get_or_create(get_self(), Config());

	trailservice::trail::ballots_table b_t(c.names.at("telos_decide_contract"), c.names.at("telos_decide_contract").value);
	auto b_itr = b_t.find(ballot_id.value);
	check(b_itr != b_t.end(), "ballot_id: " + ballot_id.to_string() + " not found.");

	trailservice::trail::treasuries_table t_t(c.names.at("telos_decide_contract"), c.names.at("telos_decide_contract").value);
	auto t_itr = t_t.find(common::S_HVOICE.code().raw());
	check(t_itr != t_t.end(), "Treasury: " + common::S_HVOICE.code().to_string() + " not found.");

	asset quorum_threshold = adjust_asset(t_itr->supply, 0.20000000);
	map<name, asset> votes = b_itr->options;
	asset votes_pass = votes.at(name("pass"));
	asset votes_fail = votes.at(name("fail"));
	asset votes_abstain = votes.at(name("abstain"));

	string debug_str = " Total Vote Weight: " + b_itr->total_raw_weight.to_string() + "\n";
	debug_str = debug_str + " Total Supply: " + t_itr->supply.to_string() + "\n";
	debug_str = debug_str + " Quorum Threshold: " + quorum_threshold.to_string() + "\n";
	debug_str = debug_str + " Votes Passing: " + votes_pass.to_string() + "\n";
	debug_str = debug_str + " Votes Failing: " + votes_fail.to_string() + "\n";
	debug_str = debug_str + " Votes Abstain: " + votes_abstain.to_string() + "\n";
	debug(debug_str);

	bool passed = false;
	if (b_itr->total_raw_weight >= quorum_threshold && 			// must meet quorum
		adjust_asset(votes_pass, 0.2500000000) > votes_fail)  	// must have 80% of the vote power
	{
		return true;
	} else {
		return false;
	}
}

void hyphadao::closeprop(const uint64_t &proposal_id)
{
	check(!is_paused(), "Contract is paused for maintenance. Please try again later.");

	object_table o_t(get_self(), name("proposal").value);
	auto o_itr = o_t.find(proposal_id);
	check(o_itr != o_t.end(), "Scope: " + name("proposal").to_string() + "; Object ID: " + std::to_string(proposal_id) + " does not exist.");
	auto prop = *o_itr;

	config_table config_s(get_self(), get_self().value);
	Config c = config_s.get_or_create(get_self(), Config());

	if (did_pass (prop.names.at("ballot_id")))
	{ 
		prop.strings["Event"] = "Proposal has passed";
		event (name("high"), variant_helper(prop.names, prop.strings, prop.assets, prop.time_points, prop.ints));

		prop.trxs.at("exec_on_approval").send(current_block_time().to_time_point().sec_since_epoch(), get_self());
	}
	else
	{
		// publish the 'failed proposal' event and change scope of the object
		prop.strings["Event"] = "Proposal has failed";
		event (name("high"), variant_helper(prop.names, prop.strings, prop.assets, prop.time_points, prop.ints));
		vector<name> new_scopes = {name("failedprops"), name("proparchive")};
		action(
			permission_level{get_self(), name("active")},
			get_self(), name("changescope"),
			std::make_tuple(name("proposal"), proposal_id, new_scopes, true))
		.send();
	}

	action(
		permission_level{get_self(), name("active")},
		c.names.at("telos_decide_contract"), name("closevoting"),
		std::make_tuple(prop.names.at("ballot_id"), true))
		.send();
}

void hyphadao::qualify_owner(const name &proposer)
{
	// Should we require that users hold Hypha before they are allowed to propose?  Disabled for now.
	// check (bank.holds_hypha (proposer), "Proposer: " + proposer.to_string() + " does not hold HYPHA.");
}

void hyphadao::payassign(const uint64_t &assignment_id, const uint64_t &period_id)
{
	check(!is_paused(), "Contract is paused for maintenance. Please try again later.");

	object_table o_t_assignment(get_self(), name("assignment").value);
	auto a_itr = o_t_assignment.find(assignment_id);
	check(a_itr != o_t_assignment.end(), "Cannot pay assignment. Assignment ID does not exist: " + std::to_string(assignment_id));

	// Only the assigned account can claim their pay; prevents inactive members from receiving tokens
	require_auth(a_itr->names.at("assigned_account"));

	object_table o_t_role(get_self(), name("role").value);
	auto r_itr = o_t_role.find(a_itr->ints.at("role_id"));
	check(r_itr != o_t_role.end(), "Cannot pay assignment. Role ID does not exist: " + std::to_string(a_itr->ints.at("role_id")));

	// Check that the assignment has not been paid for this period yet
	asspay_table asspay_t(get_self(), get_self().value);
	auto period_index = asspay_t.get_index<name("byperiod")>();
	auto per_itr = period_index.find(period_id);
	while (per_itr->period_id == period_id && per_itr != period_index.end())
	{
		check(per_itr->assignment_id != assignment_id, "Assignment ID has already been paid for this period. Period ID: " +
														   std::to_string(period_id) + "; Assignment ID: " + std::to_string(assignment_id));
		per_itr++;
	}

	// Check that the period has elapsed
	period_table period_t(get_self(), get_self().value);
	auto p_itr = period_t.find(period_id);
	check(p_itr != period_t.end(), "Cannot make payment. Period ID not found: " + std::to_string(period_id));
	check(p_itr->end_date.sec_since_epoch() < current_block_time().to_time_point().sec_since_epoch(),
		  "Cannot make payment. Period ID " + std::to_string(period_id) + " has not closed yet.");

	// Check that the creation date of the assignment is before the end of the period
	check(a_itr->created_date.sec_since_epoch() < p_itr->end_date.sec_since_epoch(),
		  "Cannot make payment to assignment. Assignment was not approved before this period.");

	// Check that pay period is between (inclusive) the start and end period of the role and the assignment
	check(a_itr->ints.at("start_period") <= period_id && a_itr->ints.at("end_period") >= period_id, "For assignment, period ID must be between " +
																										std::to_string(a_itr->ints.at("start_period")) + " and " + std::to_string(a_itr->ints.at("end_period")) + " (inclusive). You tried: " + std::to_string(period_id));

	// We've disabled this check that confirms the period being claimed falls within a role's guidelines
	// likely this will replaced with budgeting anyways
	// check(r_itr->ints.at("start_period") <= period_id && r_itr->ints.at("end_period") >= period_id, "For role, period ID must be between " +
	// 																									std::to_string(r_itr->ints.at("start_period")) + " and " + std::to_string(r_itr->ints.at("end_period")) + " (inclusive). You tried: " + std::to_string(period_id));

	float first_phase_ratio_calc = 1; // pro-rate based on elapsed % of the first phase

	// Pro-rate the payment if the assignment was created during the period being claimed
	if (a_itr->created_date.sec_since_epoch() > p_itr->start_date.sec_since_epoch())
	{
		first_phase_ratio_calc = (float)((float)p_itr->end_date.sec_since_epoch() - a_itr->created_date.sec_since_epoch()) /
								 ((float)p_itr->end_date.sec_since_epoch() - p_itr->start_date.sec_since_epoch());
	}

	// If there is an explicit ESCROW SEEDS salary amount, support sending it; else it should be calculated
	asset deferred_seeds_payment ;
	if (a_itr->assets.find("seeds_escrow_salary_per_phase") != a_itr->assets.end()) {
		deferred_seeds_payment = adjust_asset(a_itr->assets.at("seeds_escrow_salary_per_phase"), first_phase_ratio_calc);
	} else if (a_itr->assets.find("usd_salary_value_per_phase") != a_itr->assets.end()) {
		// Dynamically calculate the SEEDS amount based on the price at the end of the period being claimed
		deferred_seeds_payment = adjust_asset(	get_seeds_amount (	a_itr->assets.at("usd_salary_value_per_phase"), 
																	p_itr->end_date, 
																	get_float(a_itr->ints, "time_share_x100"),
																	get_float(a_itr->ints, "deferred_perc_x100")), 
												first_phase_ratio_calc);
	} else {
		deferred_seeds_payment = asset {0, common::S_SEEDS};
	}
	
	// If there is an explicity INSTANT SEEDS amount, support sending it, else it should be zero
	asset instant_seeds_payment ;
	if (a_itr->assets.find("seeds_instant_salary_per_phase") != a_itr->assets.end()) {
		instant_seeds_payment = adjust_asset(a_itr->assets.at("seeds_instant_salary_per_phase"), first_phase_ratio_calc);
	} else {
		instant_seeds_payment = asset {0, common::S_SEEDS};
	}

	// These values are calculated when the assignment is proposed, so simply pro-rate them if/as needed
	asset husd_payment = adjust_asset(a_itr->assets.at("husd_salary_per_phase"), first_phase_ratio_calc);
	asset voice_payment = adjust_asset(a_itr->assets.at("hvoice_salary_per_phase"), first_phase_ratio_calc);
	asset hypha_payment = adjust_asset(a_itr->assets.at("hypha_salary_per_phase"), first_phase_ratio_calc);

	// Record the payment as a whole (for the assignment/period combo); NOTE: bank also records individual payments per token
	asspay_t.emplace(get_self(), [&](auto &a) {
		a.ass_payment_id = asspay_t.available_primary_key();
		a.assignment_id = assignment_id;
		a.recipient = a_itr->names.at("assigned_account"),
		a.period_id = period_id;
		a.payment_date = current_block_time().to_time_point();
		a.payments.push_back(hypha_payment);
		a.payments.push_back(deferred_seeds_payment);
		a.payments.push_back(instant_seeds_payment);
		a.payments.push_back(voice_payment);
		a.payments.push_back(husd_payment);
	});

	string memo = "Payment for role " + std::to_string(a_itr->ints.at("role_id")) + "; Assignment ID: " + std::to_string(assignment_id) + "; Period ID: " + std::to_string(period_id);

    debug("Processing payments: HYPHA: " + hypha_payment.to_string() +
            ", DEFERRED: " + deferred_seeds_payment.to_string() +
            ", INSTANT: " + instant_seeds_payment.to_string() +
            ", HVOICE: " + voice_payment.to_string() +
            ", HUSD: " + husd_payment.to_string());

	// creating a single struct improves performance for table queries here
	asset_batch ab {};
	ab.d_seeds = deferred_seeds_payment;
	ab.hypha = hypha_payment;
	ab.seeds = instant_seeds_payment;
	ab.voice = voice_payment;
	ab.husd = husd_payment;

	ab = apply_badge_coefficients (period_id, a_itr->names.at("assigned_account"), ab);

	// Make all payments to the assigned account; NOTE: the makepayment function simply returns if the amount is zero
	// The bank knows how to send various payments/tokens
	make_payment(period_id, a_itr->names.at("assigned_account"), ab.hypha, memo, assignment_id, 1);
	make_payment(period_id, a_itr->names.at("assigned_account"), ab.d_seeds, memo, assignment_id, 0);
	make_payment(period_id, a_itr->names.at("assigned_account"), ab.seeds, memo, assignment_id, 1);
	make_payment(period_id, a_itr->names.at("assigned_account"), ab.voice, memo, assignment_id, 1);
	make_payment(period_id, a_itr->names.at("assigned_account"), ab.husd, memo, assignment_id, 1);
}