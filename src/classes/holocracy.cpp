#include "../../include/holocracy.hpp"

Holocracy::Holocracy (const name& contract) :
    role_t (contract, contract.value), 
    contract (contract),
    assignment_t (contract, contract.value),
    bank (contract) {}

void Holocracy::reset () {

    require_auth (contract);
    
    auto r_itr = role_t.begin();
    while (r_itr != role_t.end()) {
        r_itr = role_t.erase (r_itr);
    }

    auto a_itr = assignment_t.begin();
    while (a_itr != assignment_t.end()) {
        a_itr = assignment_t.erase (a_itr);
    }
}

void Holocracy::newrole (  const name& role_name, 
                            const string& description,
                            const asset& hypha_salary,
                            const asset& preseeds_salary,
                            const asset& voice_salary) {

    require_auth (contract);

    role_t.emplace (contract, [&](auto &r) {
        r.role_id           = role_t.available_primary_key();
        r.role_name         = role_name;
        r.description       = description;
        r.hypha_salary      = hypha_salary;
        r.preseeds_salary   = preseeds_salary;
        r.voice_salary      = voice_salary;
        r.created_date      = current_block_time().to_time_point();
    });
}

void Holocracy::delrole (const uint64_t& role_id) {

    require_auth (contract);

    auto ass_by_role = assignment_t.get_index<"byrole"_n>();
    check (ass_by_role.find(role_id) == ass_by_role.end(), "Delete role failed. Role has existing assignments: " + std::to_string(role_id));

    auto r_itr = role_t.find (role_id);
    check (r_itr != role_t.end(), "Delete role failed. Role does not exist: " + role_name.to_string());
    role_t.erase (r_itr);
}

void Holocracy::newassign (const name&          assigned_account,
                            const name&         role_name,
                            const string&       info_url,
                            const string&       notes,
                            const uint64_t&     start_period,
                            const float&        time_share) {
                        
    require_auth (contract);

    auto sorted_by_assigned = assignment_t.get_index<"byassigned"_n>();

    auto a_itr = sorted_by_assigned.begin();
    while (a_itr != sorted_by_assigned.end()) {
        check (a_itr->role_id != role_id, "Assigned account already has this role. Assigned account: " 
            + assigned_account.to_string() + "; Role ID: " + std::to_string(role_id));    
        a_itr++;
    }

    assignment_t.emplace (contract, [&](auto &a) {
       a.assignment_id      = assignment_t.available_primary_key();
       a.assigned_account   = assigned_account;
       a.role_id            = role_name;
       a.info_url           = info_url;
       a.notes              = notes;
       a.time_share         = time_share;
       a.start_period       = start_period;
       a.created_date       = current_block_time().to_time_point();
   });
}

void Holocracy::delassign (const uint64_t&     assignment_id) {

    require_auth (contract);

    auto a_itr = assignment_t.find (assignment_id);
    check (a_itr != assignment_t.end(), "Cannot delete assignment. Assignment ID does not exist: " + std::to_string(assignment_id));
    assignment_t.erase (a_itr);
}

asset Holocracy::adjust_asset (const asset& original_asset, const float& adjustment) {
    return asset { static_cast<int64_t> (original_asset.amount * adjustment), original_asset.symbol };
}

void Holocracy::payassign (const uint64_t& assignment_id, const uint64_t& period_id) {

    auto a_itr = assignment_t.find (assignment_id);
    check (a_itr != assignment_t.end(), "Cannot pay assignment. Assignment ID does not exist: " + std::to_string(assignment_id));

    auto r_itr = role_t.find (a_itr->role_id);
    check (r_itr != role_t.end(), "Cannot pay assignment. Role does not exist: " + a_itr->role_id);

    require_auth (a_itr->assigned_account);
    
    bank.makepayment (period_id, a_itr->assigned_account, adjust_asset(r_itr->hypha_salary, a_itr->time_share), 
        "Payment for role " + a_itr->role_name.to_string() + "; Period ID: " + std::to_string(period_id));

    bank.makepayment (period_id, a_itr->assigned_account, adjust_asset(r_itr->preseeds_salary, a_itr->time_share), 
        "Payment for role " + a_itr->role_name.to_string() + "; Period ID: " + std::to_string(period_id));
}