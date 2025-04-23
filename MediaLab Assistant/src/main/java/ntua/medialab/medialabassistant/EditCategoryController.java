package ntua.medialab.medialabassistant;

import javafx.fxml.FXML;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

public class EditCategoryController {

    @FXML
    private ChoiceBox<String> categoryChoiceBox;

    @FXML
    private TextField newCategoryNameField;

    @FXML
    private void initialize() {
        categoryChoiceBox.getItems().addAll(CategoryManager.getCategories());
    }

    @FXML
    private void handleSaveCategory() {
        String selectedCategory = categoryChoiceBox.getValue();
        String newCategoryName = newCategoryNameField.getText().trim();

        if (selectedCategory != null && !newCategoryName.isEmpty()) {
            try {
                CategoryManager.editCategory(selectedCategory, newCategoryName);
                System.out.println("Category updated: " + selectedCategory + " -> " + newCategoryName);
                handleClose();
            } catch (IllegalArgumentException e) {
                System.err.println("Error: " + e.getMessage());
            }
        } else {
            System.err.println("Error: Please select a category and enter a new name.");
        }
    }

    @FXML
    private void handleRemoveCategory() {
        String selectedCategory = categoryChoiceBox.getValue();

        if (selectedCategory != null) {
            try {

                CategoryManager.removeCategory(selectedCategory);
                System.out.println("Category removed: " + selectedCategory);
                handleClose();
            } catch (IllegalArgumentException e) {
                System.err.println("Error: " + e.getMessage());
            }
        } else {
            System.err.println("Error: Please select a category to remove.");
        }
    }

    @FXML
    private void handleClose() {
        Stage stage = (Stage) categoryChoiceBox.getScene().getWindow();
        stage.close();
    }

}