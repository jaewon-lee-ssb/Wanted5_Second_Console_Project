---
name: project-problem-notion-log
description: Review the Wanted5 second project's debugging and design-decision history, then prepare or record an evidence-based Korean entry in the fixed Notion page 프로젝트 문제·고민 기록. Use when the user asks to add, update, or organize a project problem, concern, debugging process, cause, solution, or retrospective in that page; do not use for ordinary work-log summaries.
---

# Project Problem Notion Log

Turn a real debugging or design discussion from this project into a useful problem-solving record in Notion.

## Fixed context

- Project: `Wanted5기 두번째 프로젝트`
- Repository: the repository containing this skill
- Notion page: `프로젝트 문제·고민 기록`
- Page ID: `3c402694-3834-811a-9158-ccfb69b99c7b`
- Date timezone: `Asia/Seoul`

## Gather evidence

1. Reconstruct the process from the current conversation first: the initial symptom or concern, observations, attempted approaches, rejected ideas, diagnosed cause, chosen solution, and remaining work.
2. Read relevant local code and Git history when they can confirm technical details. Treat uncommitted code as current workspace state, not as a completed or pushed solution.
3. Fetch the fixed Notion page and check whether a draft or existing record already covers the same problem. Update that record instead of creating a duplicate.
4. Separate verified facts from hypotheses. Do not invent attempts, error messages, measurements, results, code, or conclusions that are absent from the evidence. If an important detail cannot be recovered, mark it as not yet confirmed or ask one concise question when the answer would materially change the record.

## Prepare the record

Follow the page's existing Korean style and use this structure inside a dated toggle:

- `## YYYY-MM-DD — 짧은 문제 제목 {toggle="true"}`
- `### 상황`
- `### 문제·고민`
- `### 시도한 방법`
- `### 원인 가설`
- `### 결정 또는 해결`
- `### 다음 행동`
- `### 회고`

Make the title describe the technical problem, not the activity. Write concise first-person project notes that remain understandable without the original chat.

- Include relevant code snippets only when they clarify the cause or solution.
- Preserve the distinction between a suspected cause and a confirmed cause.
- For unresolved problems, state what remains unverified and give concrete next checks.
- For resolved problems, record why the chosen solution worked and what was learned.
- Do not leave generic template text such as `시도한 내용` or `후속 작업` unless it accurately represents unfinished work.
- Never store credentials, secrets, local absolute paths, or unrelated repository content in Notion.

## Preview and write boundary

Before changing Notion, show the complete proposed new or revised record and say whether it will update an existing entry or add a new one. Ask for approval for that exact write.

After approval:

1. Fetch `notion://docs/enhanced-markdown-spec` and fetch the target page again so the edit is based on its latest content.
2. Use the smallest targeted Notion page edit:
   - replace only the matching record when updating an existing entry;
   - insert a new record immediately before `# 해결된 기록` when adding one;
   - move a record into `# 해결된 기록` only when the user has indicated that it is resolved.
3. Preserve every unrelated record and page section exactly.
4. Fetch the page after writing to verify the result, then report the page link and what was added or updated.

Stop after one failed Notion write and explain the error. Do not repeatedly retry or replace the full page as a fallback.
