<?php

/**
 * @file
 * Contains \Drupal\pathauto\Form\MaillogSettingsForm.
 */

namespace Drupal\pathauto\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Component\Utility\String;

/**
 * Configure file system settings for this site.
 */
class PathautoSettingsForm extends ConfigFormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'pathauto_settings_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, array &$form_state) {
    module_load_include('inc', 'pathauto');
    $config = $this->configFactory()->get('pathauto.settings');

    $form = array();

    $form['verbose'] = array(
      '#type' => 'checkbox',
      '#title' => t('Verbose'),
      '#default_value' => $config->get('verbose'),
      '#description' => t('Display alias changes (except during bulk updates).'),
    );

    $form['separator'] = array(
      '#type' => 'textfield',
      '#title' => t('Separator'),
      '#size' => 1,
      '#maxlength' => 1,
      '#default_value' => $config->get('separator'),
      '#description' => t('Character used to separate words in titles. This will replace any spaces and punctuation characters. Using a space or + character can cause unexpected results.'),
    );

    $form['case'] = array(
      '#type' => 'radios',
      '#title' => t('Character case'),
      '#default_value' => $config->get('case'),
      '#options' => array(
        PATHAUTO_CASE_LEAVE_ASIS => t('Leave case the same as source token values.'),
        PATHAUTO_CASE_LOWER => t('Change to lower case'),
      ),
    );

    $max_length = \Drupal::service('pathauto.manager')->getAliasSchemaMaxlength();

    $form['max_length'] = array(
      '#type' => 'number',
      '#title' => t('Maximum alias length'),
      '#size' => 3,
      '#maxlength' => 3,
      '#default_value' => $config->get('max_length'),
      '#min' => 1,
      '#max' => $max_length,
      '#description' => t('Maximum length of aliases to generate. 100 is the recommended length. @max is the maximum possible length. See <a href="@pathauto-help">Pathauto help</a> for details.', array('@pathauto-help' => url('admin/help/pathauto'), '@max' => $max_length)),
    );

    $form['max_component_length'] = array(
      '#type' => 'number',
      '#title' => t('Maximum component length'),
      '#size' => 3,
      '#maxlength' => 3,
      '#default_value' => $config->get('max_component_length'),
      '#min' => 1,
      '#max' => $max_length,
      '#description' => t('Maximum text length of any component in the alias (e.g., [title]). 100 is the recommended length. @max is the maximum possible length. See <a href="@pathauto-help">Pathauto help</a> for details.', array('@pathauto-help' => url('admin/help/pathauto'), '@max' => $max_length)),
    );

    $description = t('What should Pathauto do when updating an existing content item which already has an alias?');
    if (\Drupal::moduleHandler()->moduleExists('redirect')) {
      $description .= ' ' . t('The <a href="!url">Redirect module settings</a> affect whether a redirect is created when an alias is deleted.', array('!url' => url('admin/config/search/redirect/settings')));
    }
    else {
      $description .= ' ' . t('Considering installing the <a href="!url">Redirect module</a> to get redirects when your aliases change.', array('!url' => 'http://drupal.org/project/redirect'));
    }

    $form['update_action'] = array(
      '#type' => 'radios',
      '#title' => t('Update action'),
      '#default_value' => $config->get('update_action'),
      '#options' => array(
        PATHAUTO_UPDATE_ACTION_NO_NEW => t('Do nothing. Leave the old alias intact.'),
        PATHAUTO_UPDATE_ACTION_LEAVE => t('Create a new alias. Leave the existing alias functioning.'),
        PATHAUTO_UPDATE_ACTION_DELETE => t('Create a new alias. Delete the old alias.'),
      ),
      '#description' => $description,
    );

    $form['transliterate'] = array(
      '#type' => 'checkbox',
      '#title' => t('Transliterate prior to creating alias'),
      '#default_value' => $config->get('transliterate'),
      '#description' => t('When a pattern includes certain characters (such as those with accents) should Pathauto attempt to transliterate them into the US-ASCII alphabet? Transliteration is handled by the Transliteration module.'),
    );

    $form['reduce_ascii'] = array(
      '#type' => 'checkbox',
      '#title' => t('Reduce strings to letters and numbers'),
      '#default_value' => $config->get('reduce_ascii'),
      '#description' => t('Filters the new alias to only letters and numbers found in the ASCII-96 set.'),
    );

    $form['ignore_words'] = array(
      '#type' => 'textarea',
      '#title' => t('Strings to Remove'),
      '#default_value' => $config->get('ignore_words'),
      '#description' => t('Words to strip out of the URL alias, separated by commas. Do not use this to remove punctuation.'),
      '#wysiwyg' => FALSE,
    );

    $form['punctuation'] = array(
      '#type' => 'fieldset',
      '#title' => t('Punctuation'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $punctuation = \Drupal::service('pathauto.manager')->getPunctuationCharacters();

    foreach ($punctuation as $name => $details) {
      $details['default'] = PATHAUTO_PUNCTUATION_REMOVE;
      if ($details['value'] == $config->get('separator')) {
        $details['default'] = PATHAUTO_PUNCTUATION_REPLACE;
      }
      $form['punctuation']['punctuation_' . $name] = array(
        '#type' => 'select',
        '#title' => $details['name'] . ' (<code>' . String::checkPlain($details['value']) . '</code>)',
        '#default_value' => $details['default'],
        '#options' => array(
          PATHAUTO_PUNCTUATION_REMOVE => t('Remove'),
          PATHAUTO_PUNCTUATION_REPLACE => t('Replace by separator'),
          PATHAUTO_PUNCTUATION_DO_NOTHING => t('No action (do not replace)'),
        ),
      );
    }

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, array &$form_state) {

    $config = $this->configFactory()->get('pathauto.settings');

    foreach ($form_state['values'] as $key => $value) {
      if ($key != 'submit' && $key != 'form_build_id' && $key != 'form_token' && $key != 'form_id' && $key != 'op') {
        $config->set($key, $value);
      }
    }

    $config->save();

    parent::submitForm($form, $form_state);
  }

}
