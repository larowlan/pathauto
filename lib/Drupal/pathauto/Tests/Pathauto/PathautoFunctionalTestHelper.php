<?php

namespace Drupal\pathauto\Tests\Pathauto;

/**
 * Helper test class with some added functions for testing.
 */
class PathautoFunctionalTestHelper extends PathautoTestHelper {
  protected $admin_user;

  function setUp(array $modules = array()) {
    parent::setUp($modules);

    // Set pathauto settings we assume to be as-is in this test.
    variable_set('pathauto_node_page_pattern', 'content/[node:title]');

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
    $this->admin_user = $this->drupalCreateUser($permissions);

    $this->drupalLogin($this->admin_user);
  }
}