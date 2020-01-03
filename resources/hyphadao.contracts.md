<h1 class="contract">propose</h1>
---
spec_version: "0.2.0"
title: Propose
summary: 'Create a new proposal'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} creates a new proposal to Hypha DAO with the following attributes:
names: {{to_json names}}
strings: {{to_json strings}}
assets: {{to_json assets}}
time_points: {{to_json time_points}}
ints: {{to_json ints}}
floats: {{to_json floats}}
transactions: {{to_json trxs}}


<h1 class="contract">apply</h1>

---
spec_version: "0.2.0"
title: Apply
summary: 'Make or modify an application'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} makes a new application or modifies an existing one with the content: {{content}}.


<h1 class="contract">enroll</h1>

---
spec_version: "0.2.0"
title: Enroll 
summary: 'Enroll an applicant'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} enrolls the existing applicant {{applicant}} with the content {{content}} to Hypha DAO.


<h1 class="contract">reset</h1>

---
spec_version: "0.2.0"
title: Reset
summary: 'Reset all tables within this contract'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} resets all the tables within this contract.


<h1 class="contract">resetperiods</h1>

---
spec_version: "0.2.0"
title: Reset Periods
summary: 'Reset all the periods within this contract'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} resets all the periods within this contract.


<h1 class="contract">eraseprop</h1>

---
spec_version: "0.2.0"
title: Erase Proposal
summary: 'Erase a given proposal'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} erases the proposal with the given id: {{proposal_id}} and the given type: {{proposal_type}}.


<h1 class="contract">setconfig</h1>

---
spec_version: "0.2.0"
title: Set Configuration
summary: 'Set a trail contract'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} creates or modifies the trail contract {{trail_contract}} with the hypha token contract {{hypha_token_contract}}.


<h1 class="contract">setlastballt</h1>

---
spec_version: "0.2.0"
title: Set Last Ballt
summary: 'Set the last ballot id value'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} sets the last ballot id with the value: {{last_ballot_id}}.


<h1 class="contract">addperiod</h1>

---
spec_version: "0.2.0"
title: Add Period
summary: 'Add a new period'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} add a new period with the start date: {{start_date}} and the end date: {{end_date}} with the phase: {{phase}}.


<h1 class="contract">remperiods</h1>

---
spec_version: "0.2.0"
title: Remove Periods
summary: 'Remove a given range of periods'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} removes periods from begin period: {{begin_period_id}} and end period: {{end_period_id}}.


<h1 class="contract">newrole</h1>

---
spec_version: "0.2.0"
title: New Role
summary: 'Adds a new role'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} adds a new role given the existing proposal with the id: {{proposal_id}}.


<h1 class="contract">assign</h1>

---
spec_version: "0.2.0"
title: Assign
summary: 'Adds a new assignment'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} adds a new assignment given the existing proposal with the id: {{proposal_id}}.


<h1 class="contract">makepayout</h1>

---
spec_version: "0.2.0"
title: Make Pay Out
summary: 'Makes pay out'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} pays out the quantity stored in the proposal with the id: {{proposal_id}}.


<h1 class="contract">exectrx</h1>

---
spec_version: "0.2.0"
title: Execute Transaction
summary: 'Execute pending transactions'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} executes all the pending transactions in the proposal with the id: {{proposal_id}}.


<h1 class="contract">closeprop</h1>

---
spec_version: "0.2.0"
title: Close Proposal
summary: 'Close a given proposal'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} erases the given proposal with the id: {{proposal_id}} and the type: {{proposal_type}}.


<h1 class="contract">payassign</h1>

---
spec_version: "0.2.0"
title: Pay Assignment
summary: 'Pay the given assignment'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} pays the given assignment with the id: {{assignment_id}} in the period with the id: {{period_id}}.


<h1 class="contract">addmember</h1>

---
spec_version: "0.2.0"
title: Add Member
summary: 'Add a new member'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} adds {{member}} as a New Member.


<h1 class="contract">removemember</h1>

---
spec_version: "0.2.0"
title: Remove Member
summary: 'Remove a member'
icon: http://dao-dev.hypha.earth/statics/app-logo-128x128.png#a71ba6431c13b623eccde41cd14f0f5e3ee694da7e3878407e74d8137bf89916
---

{{$action.authorization.[0].actor}} removes {{member_to_remove}} as a Member.