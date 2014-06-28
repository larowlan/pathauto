<?php

/**
 * @file
 * Contains \Drupal\pathauto\Tests\PathautoUiTest.
 */

namespace Drupal\pathauto\Tests;

use Drupal\simpletest\WebTestBase;

/**
 * Test basic pathauto functionality.
 */
class PathautoUiTest extends WebTestBase {

  use PathautoTestHelperTrait;

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = array('pathauto', 'node');

  /**
   * Admin user.
   *
   * @var \Drupal\user\UserInterface
   */
  protected $adminUser;

  /**
   * {inheritdoc}
   */
  public static function getInfo() {
    return array(
      'name' => 'Pathauto UI tests',
      'description' => 'Test the pathauto UI',
      'group' => 'Pathauto',
    );
  }

  /**
   * {inheritdoc}
   */
  function setUp() {
    parent::setUp();

    $this->drupalCreateContentType(array('type' => 'page', 'name' => 'Basic page'));
    $this->drupalCreateContentType(array('type' => 'article'));

    // Allow other modules to add additional permissions for the admin user.
    $permissions = array(
      'administer pathauto',
      'administer url aliases',
      'create url aliases',
      'administer nodes',
      'bypass node access',
      'access content overview',
    );
    $this->adminUser = $this->drupalCreateUser($permissions);
    $this->drupalLogin($this->adminUser);
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

}
