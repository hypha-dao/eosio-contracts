#include "../../include/holocracy.hpp"

Holocracy::Holocracy (const name& contract) 
    : role_t (contract, contract.value), contract (contract),
        assignment_t (contract, contract.value) {}

void holocracy::reset () {

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

void holocracy::newrole (  const name& role_name, 
                            const string& description,
                            const asset& hypha_salary,
                            const asset& preseeds_salary,
                            const asset& voice_salary) {

    require_auth (contract);

    check (role_t.find (role_name.value) == role_t.end(), "Role name already exists: " + role_name.to_string());
    role_t.emplace (contract, [&](auto &r) {
        r.role_name     = role_name;
        r.description   = description;
        r.hypha_salary  = hypha_salary;
        r.preseeds_salary   = preseeds_salary;
        r.voice_salary      = voice_salary;
        r.created_date      = current_block_time().to_time_point();
    });
}

void holocracy::delrole (const name& role_name) {

    require_auth (contract);

    auto ass_by_role = assignment_table.get_index<"byrole">();
    check (ass_by_role.find(role_name.value) == ass_by_role.end(), "Delete role failed. Role has existing assignments: " + role_name.to_string());

    auto r_itr = role_t.find (role_name.value);
    check (r_itr != role_t.end(), "Delete role failed. Role does not exist: " + role_name.to_string());
    role_t.erase (r_itr);
}

void holocracy::newassign (const name&        assigned_account,
                            const name&        role_name,
                            const string&      info_url,
                            const string&      notes,
                            const float&       time_share) {
                        
    require_auth (contract);

    assignment_t.emplace (contract, [&](auto &a) {
       a.assignment_id      = assignment_t.available_primary_keyu();
       a.assigned_account   = assigned_account;
       a.role_name          = role_name;
       a.info_url           = info_url;
       a.notes              = notes;
       a.time_share         = time_share;
       a.created_date       = current_block_time().to_time_point();
   });
}

void holocracy::delassign (const uint64_t&     assignment_id) {

    require_auth (contract);

    auto a_itr = assignment_t.find (assignment_id);
    check (a_itr != assignment_t.end(), "Cannot delete assignment. Assignment ID does not exist: " + std::to_string(assignment_id));
    assignment_t.erase (a_itr);
}

asset holocracy::adjust_asset (const asset& original_asset, const float& adjustment) {
    return asset { static_cast<int64_t> (original_asset.amount * adjustment), original_asset.symbol };
}

void holocracy::payassign (const uint64_t& assignment_id, const uint64_t& period_id) {

    require_auth (contract);

    auto a_itr = assignment_t.find (assignment_id);
    check (a_itr != assignment_t.end(), "Cannot pay assignment. Assignment ID does not exist: " + std::to_string(assignment_id));

    auto r_itr = role_t.find (a_itr->role_name.value);
    check (r_itr != role_t.end(), "Cannot pay assignment. Role does not exist: " + role_name.to_string());

    bank.makepayment (period_id, a_itr->assigned_account, adjust_asset(r_itr->hypha_salary, a_itr->time_share), 
        "Payment for role " + role_name.to_string() + "; Period ID: " + std::to_string(period_id));

    bank.makepayment (period_id, a_itr->assigned_account, adjust_asset(r_itr->preseeds_salary, a_itr->time_share), 
        "Payment for role " + role_name.to_string() + "; Period ID: " + std::to_string(period_id));
}