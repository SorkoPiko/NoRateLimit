# No Rate Limit

Prevent rate limiting by stuttering requests. This only works on Robtop's endpoints.

## Features
- Prevents rate limiting by stuttering requests to ensure no more than 90 requests (default) per minute are made.
- Configurable request limit (anywhere from 10 - 99 requests per minute, defaults to 90)

## Incompatibilities
- This mod is incompatible with GDIntercept, as it hooks the same functions. I am currently working with SMJS (GDIntercept's developer) to resolve this issue.

## Special Thanks
- [SMJS](https://github.com/SMJSGaming) - For helping me with hooking Geode Web Requests.