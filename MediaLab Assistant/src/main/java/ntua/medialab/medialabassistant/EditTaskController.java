package ntua.medialab.medialabassistant;

import javafx.fxml.FXML;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.DatePicker;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

public class EditTaskController {

    @FXML
    private TextField newTitle;

    @FXML
    private ChoiceBox<String> categoryChoice;

    @FXML
    private ChoiceBox<String> priorityChoice;

    @FXML
    private DatePicker deadlinePicker;

    @FXML
    private TextField newDescription;

    @FXML
    private ChoiceBox<String> statusPicker;

    private MedialabApplicationController mainController;
    private Task task;
    public void setMainController(MedialabApplicationController mainController) {
        this.mainController = mainController;
    }
    public void setTask(Task task) {
        this.task = task;
        newTitle.setText(task.getTitle());
        categoryChoice.setValue(task.getCategory());
        priorityChoice.setValue(task.getPriority());
        deadlinePicker.setValue(task.getDeadline());
        newDescription.setText(task.getDescription());
        statusPicker.setValue(task.getStatus().label);
    }

    @FXML
    private void initialize() {
        categoryChoice.getItems().addAll(CategoryManager.getCategories());
        priorityChoice.getItems().addAll(PriorityManager.getPriorities());
        for (Task.Status status : Task.Status.values()) {
            statusPicker.getItems().add(Task.getStatusLabel(status));
        }
    }

    @FXML
    private void handleSaveChanges() {
        task.setTitle(newTitle.getText().trim());
        task.setCategory(categoryChoice.getValue());
        task.setPriority(priorityChoice.getValue());
        task.setDeadline(deadlinePicker.getValue());
        task.setDescription(newDescription.getText().trim());
        String selectedStatusLabel = statusPicker.getValue();
        for (Task.Status status : Task.Status.values()) {
            if (Task.getStatusLabel(status).equals(selectedStatusLabel)) {
                task.setStatus(status);
                break;
            }
        }

        System.out.println("Task updated: " + task.getTitle());
        if (mainController != null) {
            mainController.updateTaskList();
            mainController.updateTaskStatistics();
            mainController.updateReminderList();
        }
        handleClose();
    }

    @FXML
    private void handleClose() {
        Stage stage = (Stage) newTitle.getScene().getWindow();
        stage.close();
    }

}
