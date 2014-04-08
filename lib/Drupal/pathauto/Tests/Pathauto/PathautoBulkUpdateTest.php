<?php

namespace Drupal\pathauto\Tests\Pathauto;

/**
 * Bulk update functionality tests.
 */
class PathautoBulkUpdateTest extends PathautoFunctionalTestHelper {
  private $nodes;

  /**
   * {@inheritdoc}
   */
  public static function getInfo() {

    return array(
      'name' => 'Pathauto bulk updating',
      'description' => 'Tests bulk updating of URL aliases.',
      'group' => 'Pathauto',
      // 'dependencies' => array('token'),
    );
  }

  function testBulkUpdate() {
    // Create some nodes.
    $this->nodes = array();
    for ($i = 1; $i <= 5; $i++) {
      $node = $this->drupalCreateNode();
      $this->nodes[$node->id()] = $node;
    }

    // Clear out all aliases.
    $this->deleteAllAliases();

    // Bulk create aliases.
    $edit = array(
      'update[node_pathauto_bulk_update_batch_process]' => TRUE,
      'update[user_pathauto_bulk_update_batch_process]' => TRUE,
    );
    $this->drupalPost('admin/config/search/path/update_bulk', '', $edit, array('Update'));
    $this->assertText('Generated 7 URL aliases.'); // 5 nodes + 2 users

    // Check that aliases have actually been created.
    foreach ($this->nodes as $node) {
      $this->assertEntityAliasExists($node);
    }
    $this->assertEntityAliasExists($this->adminUser);

    // Add a new node.
    $new_node = $this->drupalCreateNode(array('path' => array('alias' => '', 'pathauto' => FALSE)));

    // Run the update again which should only run against the new node.
    $this->drupalPost('admin/config/search/path/update_bulk', '', $edit, array('Update'));
    $this->assertText('Generated 1 URL alias.'); // 1 node + 0 users

    $this->assertEntityAliasExists($new_node);
  }
}