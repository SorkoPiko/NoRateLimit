# No Rate Limit Changelog

## 1.0.4
* Added support for a new rate limit (20 downloadGJLevel per min).
* Fixed excessive rate limiting.

## 1.0.3
* Fix Android bug. Sorry it took so long!

## 1.0.2
* Add links.

## 1.0.1
* Fix a small bug where the CCHttpClient request was freed early (Thanks [qimiko](https://github.com/qimiko)!)

## 1.0.0
* Initial release - Prevent rate limiting by stuttering requests to ensure no more than 90 requests (default) per minute are made.