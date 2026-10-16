---
name: generate-changelog
description: Generate release changelog adhering to repository standards
---

You are to write the changelog for version {{version}} of this code, following these instructions:
* The document must be in Markdown format, named `ChangeLog.md`, and located at the root of the project.
* The document must be written in English.
* Use the structure and organization of the changelogs published at the following address as a reference: #web https://github.com/JSBSim-Team/jsbsim/discussions/356.
* The changelog will cover the changes between commit {{start_commit}} and {{end_commit}} on the master branch.
* Ignore commits from the "dependabot[bot]" and "github-actions[bot]" accounts.
* For each commit, find the corresponding Pull Request (PR) number, which provides more details on the motivations behind the change. These numbers refer to the #githubRepo JSBSim-Team/jsbsim repository. You must consult these PRs to ensure that you accurately describe the intent of each change.
* Each change must be described concisely and placed under the appropriate theme (e.g., User, Python, Examples, Developer, etc.). The PR number, if available, must be included in the entry for that change.
* Each change should appear only once. If a change fits into multiple categories, choose the one most relevant or most impacted the change.
* Reference Pull Requests using plain text (e.g., `PR #XXXX`) without Markdown hyperlinks, as GitHub links PR references automatically.
* Exclude purely administrative or cosmetic commits, such as typo fixes, CI configuration updates, documentation tweaks, or formatting adjustments, unless they introduce visible behavioral changes or affect build requirements.
* Highlight any breaking changes or deprecations prominently at the top of the changelog under a dedicated "Breaking Changes" or "Important Notes" section before detailing category-specific changes.
* Do not simply copy-paste PR titles. Describe each item from an end-user or developer perspective by explaining what feature was added, what behavior changed, or what bug was resolved.
* Credit the author of each change by mentioning their GitHub handle (e.g., `@username`) alongside the PR reference, especially for community and first-time contributors.
* Start each bullet point with an imperative verb (e.g., "Add", "Fix", "Update", "Remove") and maintain consistent phrasing and punctuation throughout the document.
