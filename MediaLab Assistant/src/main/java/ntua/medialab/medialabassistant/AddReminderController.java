package ntua.medialab.medialabassistant;

import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.stage.Stage;
import javafx.util.StringConverter;

import java.time.LocalDate;
import java.time.temporal.ChronoUnit;

public class AddReminderController {

    @FXML
    private ChoiceBox<Task> tasklist;

    @FXML
    private CheckBox dayreminder;

    @FXML
    private CheckBox weekreminder;

    @FXML
    private CheckBox monthreminder;

    @FXML
    private CheckBox customreminder;

    @FXML
    private DatePicker datepicker;

    @FXML
    private Button addremindersbutton;

    @FXML
    private Button closebutton;

    @FXML
    private void initialize() {
        tasklist.getItems().addAll(TaskManager.getTasks());
        tasklist.setConverter(new StringConverter<>() {
            @Override
            public String toString(Task task) {
                return task != null ? task.getTitle() : "No Tasks";
            }
            @Override
            public Task fromString(String string) {
                return null;
            }
        });
        datepicker.setDisable(true);
        customreminder.selectedProperty().addListener((observable, oldValue, newValue) -> {
            datepicker.setDisable(!newValue);
        });
    }

    @FXML
    private void handleDayReminder() {
    }

    @FXML
    private void handleWeekReminder() {
    }

    @FXML
    private void handleMonthReminder() {
    }

    @FXML
    private void handleCustomReminder() {
        datepicker.setDisable(!customreminder.isSelected());
    }

    @FXML
    private void handleAddReminder() {
        Task selectedTask = tasklist.getValue();

        if (selectedTask != null) {
            if (selectedTask.getStatus() == Task.Status.Completed || selectedTask.getStatus() == Task.Status.Delayed) {
                System.err.println("Error: Cannot add reminders for Completed or Delayed tasks.");
                return;
            }
            if (dayreminder.isSelected()) {
                ReminderManager.scheduleReminderOneDayBefore(selectedTask);
            }
            if (weekreminder.isSelected()) {
                ReminderManager.scheduleReminderOneWeekBefore(selectedTask);
            }
            if (monthreminder.isSelected()) {
                ReminderManager.scheduleReminderOneMonthBefore(selectedTask);
            }
            if (customreminder.isSelected() && datepicker.getValue() != null) {
                int daysBefore = (int) ChronoUnit.DAYS.between(LocalDate.now(), datepicker.getValue());
                ReminderManager.scheduleReminderUserDefined(selectedTask, daysBefore);
            }
            handleClose();
        } else {
            System.err.println("Error: Please select a task.");
        }
        handleClose();
    }

    @FXML
    private void handleClose() {
        Stage stage = (Stage) closebutton.getScene().getWindow();
        stage.close();
    }
}