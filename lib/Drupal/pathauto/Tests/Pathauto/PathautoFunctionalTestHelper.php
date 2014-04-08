<?php

namespace Drupal\pathauto\Tests\Pathauto;

/**
 * Helper test class with some added functions for testing.
 */
class PathautoFunctionalTestHelper extends PathautoTestHelper {
  protected $adminUser;

  function setUp(array $modules = array()) {
    $config = \Drupal::configFactory()->get('pathauto.settings');

    parent::setUp($modules);

    // Set pathauto settings we assume to be as-is in this test.
    $config->set('node_page_pattern', 'content/[node:title]');

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
    $args = func_get_args();
    if (isset($args[1]) && is_array($args[1])) {
      $permissions = array_merge($permissions, $args[1]);
    }
    $this->adminUser = $this->drupalCreateUser($permissions);

    $this->drupalLogin($this->adminUser);
  }
}
