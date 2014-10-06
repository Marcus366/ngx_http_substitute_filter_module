A http filter module of nginx which can replace string.

===============================================================================

This module take Boyer-Moore as its replace algorithm so it is much more faster, But it is not as powerful as the current replace filter module which also support regular expression.

===============================================================================

Now it only support one command named substitue

Example as follow:

http {

    server {
        listen 80;

        location / {
            gzip on;

            substitue foo bar  # "bar" will be replaced by "foo"
        }

    }
}
