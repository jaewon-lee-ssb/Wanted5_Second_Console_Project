---
name: workspace-handoff
description: Create a self-contained handoff note when the user is moving to another workspace, computer, or Codex task and wants the next Codex instance to resume with the correct context.
---

# Workspace Handoff

Create a copyable handoff note that lets a fresh Codex task continue without access to the current conversation.

Use this skill when the user says phrases such as "작업 공간 이동할게", "집에서 이어갈게", "새 대화에서 계속할게", or asks for a progress handoff or continuation summary.

## Gather the current state

- Use the conversation as the primary source for goals, decisions, and user preferences.
- When a repository is available, inspect its current branch, status, recent commits, and relevant changed files using read-only commands. Respect all repository instructions and existing authorization boundaries.
- Separate verified completed work from planned, proposed, or unverified work. Never claim that changes were committed, pushed, built, or tested without evidence.
- Note uncommitted work and warn that connecting or cloning a repository does not transfer local-only changes or Codex conversation history.
- Do not expose secrets, tokens, credentials, or irrelevant personal information.

## Produce the handoff

Match the user's language. Keep the note concise but self-contained, including only sections that add useful context:

1. Project and current objective.
2. Completed work and verified results.
3. Current implementation state, including important files when known.
4. Decisions already made and the reasons that prevent the next Codex instance from reopening settled questions.
5. Remaining work in recommended order.
6. Known problems, blockers, risks, and constraints.
7. Repository state such as branch, latest relevant commit, uncommitted files, and push status when verified.
8. A final instruction telling the new Codex instance what to inspect first and whether it may modify files.

Deliver the handoff as a finished, reusable writing artifact that the user can paste into a new task. Do not perform commits, pushes, file edits, cloud handoffs, or other mutations unless the user separately authorizes them.

If important context cannot be verified, label it clearly as uncertain instead of guessing. If the user only needs a short transfer note, prioritize the current state, next action, and safety constraints.
