# Contributing to Omni Core

[Comments in brackets are TODOs.
The numbered lists with keyword-ish descriptions are mostly notes, and should completely be refined and replaced.
It may not the best choice to simply have some lists, but instead it would probably better to write full texts.
If you have good ideas, or want to tackle it, please just add inline comments!]

[Some introduction ... what?]

## Topics:

- [Glossary](#glossary)
- [Reporting security issues](#reporting-security-issues)
- [Reporting other issues](#reporting-other-issues)
- [Requesting features](#requesting-features)
- [Submitting and working on pull requests](#submitting-and-working-on-pull-requests)
- [Reviewing contributions](#reviewing-contributions)
- [What's next?](#whats-next)

## Glossary:

From time to time contributors or maintainers use abbreviations, and it's likely that you stumble over one of the following at some point:

1. ACK: agreement with an idea, change or submission
2. NACK: disagreement or rejection of a proposal
3. NIT: comment on an almost trivial issue
4. PR: pull request
5. WIP: work in progress
6. maintainer: project administrator, collaborator
7. contributor: someone who submits pull requests, issues, comments, ...

[The glossary may not be used at all, or maybe placed at the end?
It could be useful, given that the words are frequently used in the wild.
I'm using them in this document, too, but ideally we don't use them here.]

## Reporting security issues:

Issues with potential security implications should not be published, but instead communicated via email to [... where?].

This includes especially defects and potential exploits, and other pitfalls that may break consensus of the Omni Layer.

Please don't hesitate, if you are unsure, whether your observation qualifies as security related issue.

[It should quickly be described what happens after a report was filed.
It should be mentioned that reports can be send encrypted (list keys).
It should probably be mentioned that we confirm the receipt of a report,
but likely won't confirm, whether it was a false alarm or not.
We should mention that findings are going to be disclosed after they were fixed,
and the contributor might be notified in such a case, if desired?]

## Reporting other issues:

##### Contributors:

8. contributors should try to describe the observed behavior of the potential issue
9. if possible, including steps to reproduce an issue is usually extremely helpful
10. contributors should provide a context, if applicable, e.g. the Omni Core version, operating system, whether the issue was observed on mainnet, testnet, ...
11. stating the use of Omni Core may be helpful ("exchange operator" vs. "home user")
12. logs, or excerpts of logs, can be useful, and may be provided
13. if logs are very long, they should be embedded via [Gist](https://gist.github.com/), [Pastebin](http://pastebin.com/), ...
14. if logs are sensitive, or the contributor simply doesn't feel comfortable publishing them, they may also be provided via email to [.. where?]

[It might be noted that issues can always be submitted via email.
It might be described that we intend to resolve issues (which is kind of obvious, but still).]

##### Maintainers:

15. maintainers should react within 24-48 hours (?) (whereby a response such as "... I'm going to look into it, and will keep you updated within [some timespan]" is perfectly fine)
16. maintainers should classify and label new issues
17. it should not be assumed that either party (maintainer or contributor) is familiar with the use case or role of the other
18. to keep the contributor in the loop, maintainers should frequently communicate the progress on an issue, even if there was no substantial progress
19. issues, which are abandoned (i.e. the original contributor is gone), non-producible and likely not verifiable without further input, should (may?) be closed after [... what timespan?]

## Requesting features:

Proposals, feature requests and new ideas for Omni Core are welcome!

Changes to the Omni Layer protocol may also be discussed in the issue section of the protocol specification or on the developer mailing list:

- dev@omnilayer.org (registration necessary?)
- https://github.com/OmniLayer/spec/issues

## Submitting and working on pull requests:

[Some general words about the lifecycle of pull requests etc. ... what exactly?]

```
submission --> work in progress --> tests and peer review --> ack  --> merge
                     ^                      |             \
                     |----------------------/              -> nack --> close
                     |                                         |
                     \-----------------------------------------/
```

[This might be converted into an actual image.]

##### Contributors:

20. communication is key, and contributors should not hesitate to ask for assistance
21. consensus critical changes should be discussed with a broader audience first, which ideally includes stakeholders (e.g. Tether in the context of "send-all", or Mastercoin holders, ...)
22. pull requests should be submitted with an [expressive subject line](http://chris.beams.io/posts/git-commit/#seven-rules), which may be used for change logs
23. pull requests should include a description of what the pull request does, or intends to do (not necessarily how, which is usually visible due to the code)
24. however, complex changes, or changes that are difficult to review, should be described
25. if a pull request [resolves an issue](https://help.github.com/articles/closing-issues-via-commit-messages), it should be noted in the description
26. unrelated changes should be split into more than one submission
27. ideally code should be documented in a [doxygen compatible format](http://www.stack.nl/~dimitri/doxygen/manual/docblocks.html#cppblock)
28. tests, or a few notes and ideas about how the submission might be tested, would be awesome, but not mandatory
29. ideally commits are [GPG signed](https://git-scm.com/book/tr/v2/Git-Tools-Signing-Your-Work)
30. pull requests that don't pass the automated tests should be refined, unless failures are caused by incompatibilities inherited from the changes (in case of an API change, ...) (very rare)
31. the contributors should explicitly indicate, if a submission is still work in progress, and post an update, once the status changes, otherwise maintainers will assume the submission is ready for review, while it actually isn't
32. pending pull requests should be kept conflict free and rebased within reasonable time (what is reasonable?), unless they are on hold
33. if the submission is still "work in progress", but the contributor no longer wants to continue, then this should be communicated, so that someone else can pick it up

[It might be noted that maintainers may ask for refinements during the review.
Include other links to test resources maybe?]

##### Maintainers:

34. maintainers should welcome and encourage submissions, especially from new users
35. maintainers should label pull requests with "ready for review", "work in progress" or "on hold", and update the labels accordingly, once the status changes
maintainers may signal a first impression early, especially if it's foreseeable that a submission won't be accepted
36. maintainers should upgrade the label attached to the pull request to "ready for review", once the author considers the pull request as ready
37. stalled pull requests should either be put "on hold" or closed after [... what timespan?]

## Reviewing contributions:

[Maybe some general words on this..? If so, what?]

##### Contributors:

38. "work in progress" may be left alone until the status is upgraded, but comments are nevertheless welcome (?)
39. comments on pull requests should primarily focus on the code, and concepts should be discussed in one or more (newly created) related issues
40. nit picking is welcome, and should not considered as offense (it's by no means intended as such!)

##### Maintainers:

41. maintainers should signal the time needed for review, in case it takes longer (which threshold? how long is "longer"? 48 hours, ...?)
42. in more lengthy, or time consuming cases, maintainers, and likewise contributors, should frequently signal the ongoing progress, including some rough time estimates, and what may still be needed
43. maintainers should not ask for improvements that are not directly related to the PR (?)
44. "ready for review" marked PRs can be merged after peer-review and ACKs of at least two maintainers (unless it's really trivial) (?)
45. NACKs are perfectly fine, but should include a few words what lead to the NACK, so that contributors have a chance to either improve a PR, or gain an insight for future submissions

## What's next?

[What else?
Maybe some words to explicitly clarify what happens after the review,
or would this be more appropriate in the initial lifecycle description?
Maybe some words that contributions are very appreciated?
Maybe contributors should get some special notes in release notes etc.?
I think it would be good to include some references, for example to the other docs, or related Omni projects?
We may also note that actually using and testing the software is helpful (e.g. point to the MetaDEx test guide),
and it may also be noted that posting on Reddit etc. is also a form of contribution,
however that's probably better for the introduction.]
