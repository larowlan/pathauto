
**Description:
The pathauto module provides support functions for other modules to 
automatically generate aliases based on appropriate criteria, with a 
central settings path for site administrators.

Implementations are provided for core content types: nodes, taxonomy 
terms, users, blogs, and events.

**Benefits:

Besides making the page address more reflective of its content than
"node/138", it's important to know that modern search engines give 
heavy weight to search terms which appear in a page's URL. By 
automatically using keywords based directly on the page content in the URL, 
relevant search engine hits for your page can be significantly
enhanced.


**Installation AND Upgrades:
See the INSTALL.txt - especially step 4.

**Notices:

Urls (not) Getting Replaced With Aliases
Please bear in mind that only URLs passed through Drupal's l() or url()
functions will be replaced with their aliases during page output. If a module
or your template contains hardcoded links, such as 'href="node/$node->nid"'
those won't get replaced with their corresponding aliases. Use instead

* 'href="'. url("node/$node->nid") .'"' or
* l("Your link title", "node/$node->nid")

See http://api.drupal.org/api/HEAD/function/url and 
http://api.drupal.org/api/HEAD/function/l for more information.

Bulk Updates May Destroy Existing Aliases:
Bulk Updates may not work if your site has a large number of items to alias 
and/or if your server is particularly slow. If you are concerned about this 
problem you should backup your database (particularly the url_alias table) prior
to executing the Bulk Update. If you are interested in helping speed up this 
operation look at the Pathauto issue queue - 
http://drupal.org/project/issues/pathauto - and specifically at the issues 
http://drupal.org/node/76172 and http://drupal.org/node/67665 You can help 
provide ideas, code, and testing in those issues to make pathauto better.


**Credits:

The original module combined the functionality of Mike Ryan's autopath with
Tommy Sundstrom's path_automatic.

Significant enhancements were contributed by jdmquin @ www.bcdems.net.

Matt England added the tracker support.

Other suggestions and patches contributed by the Drupal community.

Current maintainer: Greg Knaddison (greg AT knaddison DOT com)

**Changes:
See the CHANGELOG.txt

$Id$
