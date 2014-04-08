<?php

namespace Drupal\pathauto\Tests\Pathauto;
use Drupal\Core\Session\AnonymousUserSession;

/**
 * Test basic pathauto functionality.
 */
class PathautoFunctionalTest extends PathautoFunctionalTestHelper {
  public static function getInfo() {
    return array(
      'name' => 'Pathauto basic tests',
      'description' => 'Test basic pathauto functionality.',
      'group' => 'Pathauto',
      // 'dependencies' => array('token'),
    );
  }

  /**
   * Basic functional testing of Pathauto.
   */
  function testNodeEditing() {
    $config = \Drupal::configFactory()->get('pathauto.settings');

    // Delete the default node pattern. Only the page content type will have a pattern.
    $config->clear('node_pattern');

    // Ensure that the Pathauto checkbox is checked by default on the node add form.
    $this->drupalGet('node/add/page');
    $this->assertFieldChecked('edit-path-pathauto');

    // Create node for testing by previewing and saving the node form.
    $title = ' Testing: node title [';
    $automatic_alias = 'content/testing-node-title';
    //$this->drupalPost(NULL, '', array('title' => $title), array('Preview'));
    $this->drupalPost('node/add/page', '', array('title' => $title), array('Save'));
    $node = $this->drupalGetNodeByTitle($title);

    // Look for alias generated in the form.
    $this->drupalGet("{$node->id()}/edit");
    $this->assertFieldChecked('edit-path-pathauto');
    $this->assertFieldByName('path[alias]', $automatic_alias, 'Generated alias visible in the path alias field.');

    // Check whether the alias actually works.
    $this->drupalGet($automatic_alias);
    $this->assertText($title, 'Node accessible through automatic alias.');

    // Manually set the node's alias.
    $manual_alias = 'content/' . $node->id();
    $edit = array(
      'path[pathauto]' => FALSE,
      'path[alias]' => $manual_alias,
    );
    $this->drupalPost("{$node->getSystemPath()}/edit", $edit, t('Save'));
    $this->assertText("Basic page $title has been updated.");

    // Check that the automatic alias checkbox is now unchecked by default.
    $this->drupalGet("node/{$node->id()}/edit");
    $this->assertNoFieldChecked('edit-path-pathauto');
    $this->assertFieldByName('path[alias]', $manual_alias);

    // Submit the node form with the default values.
    $this->drupalPost(NULL, array(), t('Save'));
    $this->assertText("Basic page $title has been updated.");

    // Test that the old (automatic) alias has been deleted and only accessible
    // through the new (manual) alias.
    $this->drupalGet($automatic_alias);
    $this->assertResponse(404, 'Node not accessible through automatic alias.');
    $this->drupalGet($manual_alias);
    $this->assertText($title, 'Node accessible through manual alias.');

    // Now attempt to create a node that has no pattern (article content type).
    // The Pathauto checkbox should not exist.
    $this->drupalGet('node/add/article');
    $this->assertNoFieldById('edit-path-pathauto');
    $this->assertFieldByName('path[alias]', '');

    $edit = array();
    $edit['title'] = 'My test article';
    $this->drupalPost(NULL, $edit, t('Save'));
    $node = $this->drupalGetNodeByTitle($edit['title']);

    // Pathauto checkbox should still not exist.
    $this->drupalGet($node->getSystemPath() . '/edit');
    $this->assertNoFieldById('edit-path-pathauto');
    $this->assertFieldByName('path[alias]', '');
    $this->assertNoEntityAlias($node);
  }

  /**
   * Test node operations.
   */
  function testNodeOperations() {
    $node1 = $this->drupalCreateNode(array('title' => 'node1'));
    $node2 = $this->drupalCreateNode(array('title' => 'node2'));

    // Delete all current URL aliases.
    $this->deleteAllAliases();

    $edit = array(
      'operation' => 'pathauto_update_alias',
      "nodes[{$node1->id()}]" => TRUE,
    );
    $this->drupalPost('admin/content', $edit, t('Update'));
    $this->assertText('Updated URL alias for 1 node.');

    $this->assertEntityAlias($node1, 'content/' . $node1->getTitle());
    $this->assertEntityAlias($node2, 'node/' . $node2->id());
  }

  /**
   * Basic functional testing of Pathauto with taxonomy terms.
   */
  function testTermEditing() {
    $this->drupalGet('admin/structure');
    $this->drupalGet('admin/structure/taxonomy');

    // Create term for testing.
    $name = ' Testing: term name [ ';
    $automatic_alias = 'tags/testing-term-name';
    $this->drupalPost('admin/structure/taxonomy/tags/add', array('name' => $name), 'Save');
    $name = trim($name);
    $this->assertText("Created new term $name.");
    $term = $this->drupalGetTermByName($name);

    // Look for alias generated in the form.
    $this->drupalGet("taxonomy/term/{$term->tid}/edit");
    $this->assertFieldChecked('edit-path-pathauto');
    $this->assertFieldByName('path[alias]', $automatic_alias, 'Generated alias visible in the path alias field.');

    // Check whether the alias actually works.
    $this->drupalGet($automatic_alias);
    $this->assertText($name, 'Term accessible through automatic alias.');

    // Manually set the term's alias.
    $manual_alias = 'tags/' . $term->tid;
    $edit = array(
      'path[pathauto]' => FALSE,
      'path[alias]' => $manual_alias,
    );
    $this->drupalPost("taxonomy/term/{$term->tid}/edit", $edit, t('Save'));
    $this->assertText("Updated term $name.");

    // Check that the automatic alias checkbox is now unchecked by default.
    $this->drupalGet("taxonomy/term/{$term->tid}/edit");
    $this->assertNoFieldChecked('edit-path-pathauto');
    $this->assertFieldByName('path[alias]', $manual_alias);

    // Submit the term form with the default values.
    $this->drupalPost(NULL, array(), t('Save'));
    $this->assertText("Updated term $name.");

    // Test that the old (automatic) alias has been deleted and only accessible
    // through the new (manual) alias.
    $this->drupalGet($automatic_alias);
    $this->assertResponse(404, 'Term not accessible through automatic alias.');
    $this->drupalGet($manual_alias);
    $this->assertText($name, 'Term accessible through manual alias.');
  }

  /**
   * Basic functional testing of Pathauto with users.
   */
  function testUserEditing() {
    // There should be no Pathauto checkbox on user forms.
    $this->drupalGet('user/' . $this->adminUser->uid . '/edit');
    $this->assertNoFieldById('edit-path-pathauto');
  }

  /**
   * Test user operations.
   */
  function testUserOperations() {
    $account = $this->drupalCreateUser();

    // Delete all current URL aliases.
    $this->deleteAllAliases();

    $edit = array(
      'operation' => 'pathauto_update_alias',
      "accounts[{$account->id()}]" => TRUE,
    );
    $this->drupalPost('admin/people', $edit, t('Update'));
    $this->assertText('Updated URL alias for 1 user account.');

    $this->assertEntityAlias($account, 'users/' . drupal_strtolower($account->getName()));
    $this->assertEntityAlias($this->adminUser, 'user/' . $this->adminUser->id());
  }

  function testSettingsValidation() {
    $edit = array();
    $edit['pathauto_max_length'] = 'abc';
    $edit['pathauto_max_component_length'] = 'abc';
    $this->drupalPost('admin/config/search/path/settings', $edit, 'Save configuration');
    $this->assertText('The field Maximum alias length is not a valid number.');
    $this->assertText('The field Maximum component length is not a valid number.');
    $this->assertNoText('The configuration options have been saved.');

    $edit['pathauto_max_length'] = '0';
    $edit['pathauto_max_component_length'] = '0';
    $this->drupalPost('admin/config/search/path/settings', $edit, 'Save configuration');
    $this->assertText('The field Maximum alias length cannot be less than 1.');
    $this->assertText('The field Maximum component length cannot be less than 1.');
    $this->assertNoText('The configuration options have been saved.');

    $edit['pathauto_max_length'] = '999';
    $edit['pathauto_max_component_length'] = '999';
    $this->drupalPost('admin/config/search/path/settings', $edit, 'Save configuration');
    $this->assertText('The field Maximum alias length cannot be greater than 255.');
    $this->assertText('The field Maximum component length cannot be greater than 255.');
    $this->assertNoText('The configuration options have been saved.');

    $edit['pathauto_max_length'] = '50';
    $edit['pathauto_max_component_length'] = '50';
    $this->drupalPost('admin/config/search/path/settings', $edit, 'Save configuration');
    $this->assertText('The configuration options have been saved.');
  }

  function testPatternsValidation() {
    $edit = array();
    $edit['pathauto_node_pattern'] = '[node:title]/[user:name]/[term:name]';
    $edit['pathauto_node_page_pattern'] = 'page';
    $this->drupalPost('admin/config/search/path/patterns', $edit, 'Save configuration');
    $this->assertText('The Default path pattern (applies to all content types with blank patterns below) is using the following invalid tokens: [user:name], [term:name].');
    $this->assertText('The Pattern for all Basic page paths cannot contain fewer than one token.');
    $this->assertNoText('The configuration options have been saved.');

    $edit['pathauto_node_pattern'] = '[node:title]';
    $edit['pathauto_node_page_pattern'] = 'page/[node:title]';
    $edit['pathauto_node_article_pattern'] = '';
    $this->drupalPost('admin/config/search/path/patterns', $edit, 'Save configuration');
    $this->assertText('The configuration options have been saved.');
  }

  /**
   * Test programmatic entity creation for aliases.
   */
  function testProgrammaticEntityCreation() {
    $node = $this->drupalCreateNode(array('title' => 'Test node', 'path' => array('pathauto' => TRUE)));
    $this->assertEntityAlias($node, 'content/test-node');

    $vocabulary = $this->addVocabulary(array('name' => 'Tags'));
    $term = $this->addTerm($vocabulary, array('name' => 'Test term', 'path' => array('pathauto' => TRUE)));
    $this->assertEntityAlias($term, 'tags/test-term');

    $edit['name'] = 'Test user';
    $edit['mail'] = 'test-user@example.com';
    $edit['pass']   = user_password();
    $edit['path'] = array('pathauto' => TRUE);
    $edit['status'] = 1;
    $account = entity_create('user', $edit);
    $this->assertEntityAlias($account, 'users/test-user');
  }
}
