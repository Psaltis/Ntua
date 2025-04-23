package ntua.medialab.medialabassistant;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.DatePicker;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

import java.time.LocalDate;

public class AddTaskController {

    @FXML
    private TextField Title;

    @FXML
    private ChoiceBox<String> categoryChoice;

    @FXML
    private ChoiceBox<String> priorityChoice;

    @FXML
    private DatePicker deadlinePicker;

    @FXML
    private TextField Description;

    @FXML
    private void initialize() {
        categoryChoice.getItems().addAll(CategoryManager.getCategories());
        priorityChoice.getItems().addAll(PriorityManager.getPriorities());
    }

    @FXML
    private void handleAddTask() {
        String title = Title.getText().trim();
        String category = categoryChoice.getValue();
        String priority = priorityChoice.getValue();
        LocalDate deadline = deadlinePicker.getValue();
        String description = Description.getText().trim();

        if (!title.isEmpty() && category != null && priority != null) {
            Task task = new Task(title, description, category, priority, deadline, Task.Status.Open);
            TaskManager.addTask(task);
            System.out.println("Task added: " + title);
            handleClose();
        } else {
            System.err.println("Error: Please fill in all required fields.");
        }
    }

    @FXML
    private void handleClose() {
        Stage stage = (Stage) Title.getScene().getWindow();
        stage.close();
    }

}
