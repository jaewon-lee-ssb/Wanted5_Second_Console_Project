---
name: workspace-handoff
description: Create and publish a self-contained handoff note to the Wanted5 second project's fixed Notion handoff page when the user is moving to another workspace, computer, or Codex task.
---

# Workspace Handoff

Create a self-contained handoff note that lets a fresh Codex task continue without access to the current conversation, then publish it to the project's fixed Notion page.

Use this skill when the user says phrases such as "작업 공간 이동할게", "집에서 이어갈게", "새 대화에서 계속할게", or asks for a progress handoff or continuation summary.

## Gather the current state

- Use the conversation as the primary source for goals, decisions, and user preferences.
- When a repository is available, inspect its current branch, status, recent commits, and relevant changed files using read-only commands. Respect all repository instructions and existing authorization boundaries.
- Separate verified completed work from planned, proposed, or unverified work. Never claim that changes were committed, pushed, built, or tested without evidence.
- Note uncommitted work and warn that connecting or cloning a repository does not transfer local-only changes or Codex conversation history.
- Do not expose secrets, tokens, credentials, or irrelevant personal information.

## Fixed Notion target

- Parent project: `Wanted5기 두번째 프로젝트`
- Parent page ID: `3c402694-3834-8114-aa90-fd01e4169ba4`
- Handoff page: `작업 인계`
- Handoff page ID: `3ca02694-3834-80a9-8bb0-e0ac60890e90`
- Handoff page URL: `https://app.notion.com/p/3ca02694383480a98bb0e0ac60890e90?pvs=204`

Use the page ID rather than searching by title so that the similarly named legacy page `작업 현황 인계` is never changed accidentally. Fetch the fixed page before every update and verify that its title and parent still match the values above. If they do not match, stop without writing and explain the mismatch.

## Produce and publish the handoff

Match the user's language. Keep the note concise but self-contained, including only sections that add useful context:

1. Project and current objective.
2. Completed work and verified results.
3. Current implementation state, including important files when known.
4. Decisions already made and the reasons that prevent the next Codex instance from reopening settled questions.
5. Remaining work in recommended order.
6. Known problems, blockers, risks, and constraints.
7. Repository state such as branch, latest relevant commit, uncommitted files, and push status when verified.
8. A final instruction telling the new Codex instance what to inspect first and whether it may modify files.

Write the finished handoff into the fixed `작업 인계` page using a full-content replacement. Each invocation supersedes the previous handoff: do not append, preserve stale handoff sections, create another page, or update the legacy `작업 현황 인계` page. Invoking this skill is authorization to replace the body of this fixed page with the newly gathered handoff; it does not authorize any other Notion mutation.

Before replacing content, inspect the existing page for child pages or databases. If replacement would delete child content or requires an `allow_deleting_content` override, stop and ask the user instead of enabling deletion. Stop after one failed Notion write and report the error without retrying. After a successful update, fetch the page once to verify the new handoff and return its link to the user.

Do not perform commits, pushes, repository file edits, cloud handoffs, or other mutations unless the user separately authorizes them.

If important context cannot be verified, label it clearly as uncertain instead of guessing. If the user only needs a short transfer note, prioritize the current state, next action, and safety constraints.
