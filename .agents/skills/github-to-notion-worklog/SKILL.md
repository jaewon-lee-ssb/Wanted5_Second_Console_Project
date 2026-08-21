---
name: github-to-notion-worklog
description: Read today's committed work from the local Wanted5 Second Console Project repository with git commands, summarize the commits and changed files, and prepare or record the work in the user's second-project Notion task tracker. Use when the user asks to log, sync, or summarize today's local Git work into that tracker; do not use for general Git summaries or unrelated Notion databases.
---

# GitHub to Notion Worklog

Turn the user's pushed work into concise, evidence-based entries in the Notion database `두번째 프로젝트 작업 트래커`.

## Model routing

Run this workflow with a subagent using model `gpt-5.6-luna`. Give it this skill and the current user request, and keep the external-write preview and approval boundary in this skill. If Luna delegation is unavailable, explain that before continuing with another model.

## Fixed targets

- Repository: `jaewon-lee-ssb/Wanted5_Second_Console_Project`
- Notion database: `두번째 프로젝트 작업 트래커`
- Data source: `collection://3c202694-3834-8073-ac2f-000b97af2e8d`
- Timezone for “today”: `Asia/Seoul`

## Workflow

1. Use local `git` commands to read commits for the requested date; default to today in `Asia/Seoul`. Do not call GitHub tools or fetch from the network. Use the current branch by default. When the user specifically asks for pushed work, inspect its `origin/<branch>` remote-tracking ref and clearly note that it reflects the latest locally known remote state.
2. Confirm the repository identity from `git remote get-url origin`. Do not summarize a different remote, uncommitted working-tree changes, or commits from another branch unless the user explicitly asks.
3. Inspect commit messages, changed files, and relevant diffs. Group commits that implement one outcome into one work item; keep unrelated outcomes separate. Do not infer work that is not supported by the Git evidence.
4. Check the target Notion data source for entries covering the same commit hashes or work outcome. Avoid duplicates; propose updating an existing entry when appropriate.
5. Prepare a preview before any Notion write. For each proposed item show the title, description, status, type, priority, commit date, and supporting commits. Ask for the user's approval to create or update exactly those entries.
6. Only after approval, write to the fixed data source and report the resulting Notion page links. Stop after one failed write and explain the error; do not repeatedly retry an external mutation.

## Property mapping

- `작업 이름`: short outcome-oriented Korean title based on the commits.
- `설명`: Korean summary of what changed and the important files or components. Do not include commit hashes or commit links in this property.
- `상태`: `완료` for work already pushed, unless the Git evidence clearly says it remains in progress.
- `작업 유형`: inspect the current multi-select options and reuse the most appropriate existing label. If none fits, add one concise Korean label while preserving every existing option, then assign it to the work item.
- `우선순위`: leave empty unless the user or Git evidence establishes `높음`, `보통`, or `낮음`.
- `마감일`: use the commit date in `Asia/Seoul`. For a grouped item with multiple commits, use the latest commit date.
- `담당자`: always assign the current authenticated Notion user. Resolve the current user rather than hard-coding a person ID.

Do not populate the `노력 수준` property, even if it remains present in the tracker schema.

Keep summaries concise and useful as a work log. Never store credentials, secrets, local absolute paths, or unrelated repository content in Notion.
