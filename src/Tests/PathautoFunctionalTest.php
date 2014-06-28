<?php

/**
 * @file
 * Contains \Drupal\pathauto\Tests\PathautoFunctionalTest.
 */

namespace Drupal\pathauto\Tests;
use Drupal\views\Views;

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
    $this->assertFieldChecked('edit-path-0-pathauto');

    // Create node for testing by previewing and saving the node form.
    $title = ' Testing: node title [';
    $automatic_alias = 'testing-node-title';
    $this->drupalCreateNode(array('title' => $title));
    /*$this->drupalPostForm(NULL, array('title' => $title), array('Preview'));
    $this->drupalPostForm(NULL, array(), 'Save');*/
    $node = $this->drupalGetNodeByTitle($title);

    // Look for alias generated in the form.
    $this->drupalGet("node/{$node->id()}/edit");
    $this->assertFieldChecked('edit-path-0-pathauto');
    $this->assertFieldByName('path[0][alias]', $automatic_alias, 'Generated alias visible in the path alias field.');

    // Check whether the alias actually works.
    $this->drupalGet($automatic_alias);
    $this->assertText($title, 'Node accessible through automatic alias.');

    // Manually set the node's alias.
    $manual_alias = 'content/' . $node->id();
    $edit = array(
      'path[0][pathauto]' => FALSE,
      'path[0][alias]' => $manual_alias,
    );
    $this->drupalPostForm("{$node->getSystemPath()}/edit", $edit, t('Save and keep published'));
    $this->assertRaw(t('@type %title has been updated.', array('@type' => 'page', '%title' => $title)));

    // Check that the automatic alias checkbox is now unchecked by default.
    $this->drupalGet("node/{$node->id()}/edit");
    $this->assertNoFieldChecked('edit-path-0-pathauto');
    $this->assertFieldByName('path[0][alias]', $manual_alias);

    // Submit the node form with the default values.
    $this->drupalPostForm(NULL, array('path[0][pathauto]' => FALSE), t('Save and keep published'));
    $this->assertRaw(t('@type %title has been updated.', array('@type' => 'page', '%title' => $title)));

    // Test that the old (automatic) alias has been deleted and only accessible
    // through the new (manual) alias.
    $this->drupalGet($automatic_alias);
    $this->assertResponse(404, 'Node not accessible through automatic alias.');
    $this->drupalGet($manual_alias);
    $this->assertText($title, 'Node accessible through manual alias.');

    // Now attempt to create a node that has no pattern (article content type).
    // The Pathauto checkbox should not exist.
    $this->drupalGet('node/add/article');
    $this->assertNoFieldById('edit-path-0-pathauto');
    $this->assertFieldByName('path[0][alias]', '');

    $edit = array();
    $edit['title'] = 'My test article';
    $this->drupalCreateNode($edit);
    //$this->drupalPostForm(NULL, $edit, t('Save and keep published'));
    $node = $this->drupalGetNodeByTitle($edit['title']);

    // Pathauto checkbox should still not exist.
    $this->drupalGet($node->getSystemPath() . '/edit');
    $this->assertNoFieldById('edit-path-0-pathauto');
    $this->assertFieldByName('path[0][alias]', '');
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
      'action' => 'pathauto_update_alias_node',
      // @todo - here we expect the $node1 to be at 0 position, any better way?
      'node_bulk_form[0]' => TRUE,
    );
    $this->drupalPostForm('admin/content', $edit, t('Apply'));
    $this->assertRaw(\Drupal::translation()->formatPlural(1, '%action was applied to @count item.', '%action was applied to @count items.', array(
      '%action' => 'Update URL-Alias',
    )));

    $this->assertEntityAlias($node1, $node1->getTitle());
    $this->assertEntityAlias($node2, 'node/' . $node2->id());
  }

  /**
   * Basic functional testing of Pathauto with taxonomy terms.
   */
  function testTermEditing() {
    $this->drupalGet('admin/structure');
    $this->drupalGet('admin/structure/taxonomy');

    // Add vocabulary "tags".
    $vocabulary = $this->addVocabulary(array('name' => 'tags', 'vid' => 'tags'));

    // Create term for testing.
    $name = 'Testing: term name [';
    $automatic_alias = 'tags/testing-term-name';
    $this->drupalPostForm('admin/structure/taxonomy/manage/tags/add', array('name[0][value]' => $name), 'Save');
    $name = trim($name);
    $this->assertText("Created new term $name.");
    $term = $this->drupalGetTermByName($name);

    // Look for alias generated in the form.
    $this->drupalGet("taxonomy/term/{$term->id()}/edit");
    $this->assertFieldChecked('edit-path-0-pathauto');
    $this->assertFieldByName('path[0][alias]', $automatic_alias, 'Generated alias visible in the path alias field.');

    // Check whether the alias actually works.
    $this->drupalGet($automatic_alias);
    $this->assertText($name, 'Term accessible through automatic alias.');

    // Manually set the term's alias.
    $manual_alias = 'tags/' . $term->id();
    $edit = array(
      'path[0][pathauto]' => FALSE,
      'path[0][alias]' => $manual_alias,
    );
    $this->drupalPostForm("taxonomy/term/{$term->id()}/edit", $edit, t('Save'));
    $this->assertText("Updated term $name.");

    // Check that the automatic alias checkbox is now unchecked by default.
    $this->drupalGet("taxonomy/term/{$term->id()}/edit");
    $this->assertNoFieldChecked('edit-path-0-pathauto');
    $this->assertFieldByName('path[0][alias]', $manual_alias);

    // Submit the term form with the default values.
    $this->drupalPostForm(NULL, array('path[0][pathauto]' => FALSE), t('Save'));
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
    $this->drupalGet('user/' . $this->adminUser->id() . '/edit');
    $this->assertNoFieldById('path[0][pathauto]');
  }

  /**
   * Test user operations.
   */
  function testUserOperations() {
    $account = $this->drupalCreateUser();

    // Delete all current URL aliases.
    $this->deleteAllAliases();

    // Find the position of just created account in the user_admin_people view.
    $view = Views::getView('user_admin_people');
    $view->initDisplay();
    $view->preview('page_1');
    foreach ($view->result as $key => $row) {
      if ($row->users_name == $account->getUsername()) {
        break;
      }
    }

    $edit = array(
      'action' => 'pathauto_update_alias_user',
      "user_bulk_form[$key]" => TRUE,
    );
    $this->drupalPostForm('admin/people', $edit, t('Apply'));
    $this->assertRaw(\Drupal::translation()->formatPlural(1, '%action was applied to @count item.', '%action was applied to @count items.', array(
      '%action' => 'Update URL-Alias',
    )));

    $this->assertEntityAlias($account, 'users/' . drupal_strtolower($account->getUsername()));
    $this->assertEntityAlias($this->adminUser, 'user/' . $this->adminUser->id());
  }

  function testSettingsValidation() {
    $edit = array();
    $edit['max_length'] = 'abc';
    $edit['max_component_length'] = 'abc';
    $this->drupalPostForm('admin/config/search/path/settings', $edit, 'Save configuration');
    /*$this->assertText('The field Maximum alias length is not a valid number.');
    $this->assertText('The field Maximum component length is not a valid number.');*/
    $this->assertNoText('The configuration options have been saved.');

    $edit['max_length'] = '0';
    $edit['max_component_length'] = '0';
    $this->drupalPostForm('admin/config/search/path/settings', $edit, 'Save configuration');
    /*$this->assertText('The field Maximum alias length cannot be less than 1.');
    $this->assertText('The field Maximum component length cannot be less than 1.');*/
    $this->assertNoText('The configuration options have been saved.');

    $edit['max_length'] = '999';
    $edit['max_component_length'] = '999';
    $this->drupalPostForm('admin/config/search/path/settings', $edit, 'Save configuration');
    /*$this->assertText('The field Maximum alias length cannot be greater than 255.');
    $this->assertText('The field Maximum component length cannot be greater than 255.');*/
    $this->assertNoText('The configuration options have been saved.');

    $edit['max_length'] = '50';
    $edit['max_component_length'] = '50';
    $this->drupalPostForm('admin/config/search/path/settings', $edit, 'Save configuration');
    $this->assertText('The configuration options have been saved.');
  }

  function testPatternsValidation() {
    $edit = array();
    $this->drupalGet('admin/config/search/path/patterns');
    $edit['node[_default]'] = '[node:title]/[user:name]/[term:name]';
    $edit['node[page][_default]'] = 'page';
    $this->drupalPostForm('admin/config/search/path/patterns', $edit, 'Save configuration');
    $this->assertText('The Default path pattern (applies to all content types with blank patterns below) is using the following invalid tokens: [user:name], [term:name].');
    $this->assertText('The Pattern for all Basic page paths cannot contain fewer than one token.');
    $this->assertNoText('The configuration options have been saved.');

    $edit['node[_default]'] = '[node:title]';
    $edit['node[page][_default]'] = 'page/[node:title]';
    $edit['node[article][_default]'] = '';
    $this->drupalPostForm('admin/config/search/path/patterns', $edit, 'Save configuration');
    $this->assertText('The configuration options have been saved.');
  }

  /**
   * Test programmatic entity creation for aliases.
   */
  function testProgrammaticEntityCreation() {
    $node = $this->drupalCreateNode(array('title' => 'Test node', 'path' => array('pathauto' => TRUE)));
    $this->assertEntityAlias($node, 'test-node');

    $vocabulary = $this->addVocabulary(array('name' => 'Tags'));
    $term = $this->addTerm($vocabulary, array('name' => 'Test term', 'path' => array('pathauto' => TRUE)));
    $this->assertEntityAlias($term, 'tags/test-term');

    $edit['name'] = 'Test user';
    $edit['mail'] = 'test-user@example.com';
    $edit['pass']   = user_password();
    $edit['path'] = array('pathauto' => TRUE);
    $edit['status'] = 1;
    $account = entity_create('user', $edit);
    $account->save();
    $this->assertEntityAlias($account, 'users/test-user');
  }
}
