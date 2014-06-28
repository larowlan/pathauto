<?php

/**
 * @file
 * Contains \Drupal\pathauto\Tests\PathautoFunctionalTestHelper.
 */

namespace Drupal\pathauto\Tests;
use Drupal\simpletest\WebTestBase;

/**
 * Helper test class with some added functions for testing.
 */
class PathautoFunctionalTestHelper extends WebTestBase {
  use PathautoTestHelperTrait;

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = array('path', 'token', 'pathauto', 'taxonomy', 'views');

  protected $adminUser;

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
      'administer taxonomy',
      'administer users',
    );
    $this->adminUser = $this->drupalCreateUser($permissions);
    $this->drupalLogin($this->adminUser);
  }
}
