#include <hyphadac.hpp>

ACTION hyphadac::reset () {}

void hyphadac::setconfig (const uint32_t period_length, 
                           const name     hypha_token_contract) {

   require_auth (get_self());
   check (is_account(hypha_token_contract), "HYPHA token contract is not an account: " + hypha_token_contract.to_string());

   config_table config_s (get_self(), get_self().value);
   Config c = config_s.get_or_create (get_self(), Config());
   c.period_length         = period_length;
   c.hypha_token_contract  = hypha_token_contract;
   c.voice_token_contract  = hypha_token_contract;
   c.preseeds_token_contract = hypha_token_contract;
   config_s.set(c, get_self());
}

void hyphadac::newrole (const name role_name, 
                        const string description,
                        const asset hypha_salary,
                        const asset preseeds_salary,
                        const asset voice_salary) {

   require_auth (get_self());
   role_table role_t (get_self(), get_self().value);
   check (role_t.find (role_name.value) == role_t.end(), "Role already exists: " + role_name.to_string());

   role_t.emplace (get_self(), [&](auto &r) {
      r.role_name    = role_name;
      r.description  = description;
      r.hypha_salary = hypha_salary;
      r.preseeds_salary = preseeds_salary;
      r.voice_salary    = voice_salary;

      // TODO: is current_block_time in milliseconds or seconds, and since when
      r.created_date    = current_block_time().to_time_point();
      r.updated_date    = current_block_time().to_time_point();
   });
}

void hyphadac::updaterole (const name role_name, 
                           const string description,
                           const asset hypha_salary,
                           const asset preseeds_salary,
                           const asset voice_salary) {
   require_auth (get_self());
   role_table role_t (get_self(), get_self().value);
   
   auto r_itr = role_t.find (role_name.value);
   check (r_itr != role_t.end(), "Role does not exist: " + role_name.to_string());

   role_t.modify (r_itr, get_self(), [&](auto &r) {
      r.description  = description;
      r.hypha_salary = hypha_salary;
      r.preseeds_salary = preseeds_salary;
      r.voice_salary    = voice_salary;

      // TODO: is current_block_time in milliseconds or seconds, and since when
      r.updated_date    = current_block_time().to_time_point();
   });
}

void hyphadac::assign (const name     assigned_account,
                        const name     role_name,
                        const time_point start_date,
                        const float    time_share) {
   require_auth (get_self());
   assignment_table assignment_t (get_self(), get_self().value);
   auto sorted_by_assigned = assignment_t.get_index<"byassigned"_n>();

   auto a_itr = sorted_by_assigned.begin();
   while (a_itr != sorted_by_assigned.end()) {
      check (a_itr->role_name != role_name, "Assigned account already has this role. Assigned account: " 
         + assigned_account.to_string() + "; Role name: " + role_name.to_string());    
      a_itr++;
   }
   
   assignment_t.emplace (get_self(), [&](auto &a) {
      a.assignment_id      = assignment_t.available_primary_key();
      a.assigned_account   = assigned_account;
      a.role_name          = role_name;
      a.start_date         = start_date;
      a.time_share         = time_share;
   });
}

void hyphadac::contribute (const name     contributor,
                           const string   description,
                           const asset    hypha_value,
                           const asset    preseeds_value, 
                           const time_point contribution_date) {

   require_auth (get_self());
   contribution_table contribution_t (get_self(), get_self().value);

   contribution_t.emplace (get_self(), [&](auto &c) {
      c.contribution_id       = contribution_t.available_primary_key();
      c.contributor           = contributor;
      c.description           = description;
      c.hypha_value           = hypha_value;
      c.preseeds_value        = preseeds_value;
      c.contribution_date     = contribution_date;
      c.status                = common::OPEN;
      c.created_date          = current_block_time().to_time_point();
   });
}