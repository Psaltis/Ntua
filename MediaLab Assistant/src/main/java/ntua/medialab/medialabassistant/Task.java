package ntua.medialab.medialabassistant;

import java.time.LocalDate;
import java.io.*;
import java.util.List;

public class Task {
    private String title;
    private String description;
    private String category;
    private String priority;
    private LocalDate deadline;
    private Status status;
    public enum Status{
        Open("Open"), InProgress("In Progress"), Postponed("Postponed"), Completed("Completed"), Delayed("Delayed");
        public final String label;
        Status(String label){
            this.label = label;
        }
    }
    public static String getStatusLabel(Task.Status status) {
        switch (status) {
            case InProgress:
                return "In progress";
            case Completed:
                return "Completed";
            case Delayed:
                return "Delayed";
            default:
                return status.toString();
        }
    }
    public Task (String title, String description, String category, String priority, LocalDate deadline, Status status){
        this.title = title;
        this.description = description;
        setCategory(category);
        setPriority(priority);
        this.deadline = deadline;
        this.status = status;
    }
    public String getTitle() {
        return title;
    }
    public void setTitle(String title) {
        this.title = title;
    }
    public String getDescription() {
        return description;
    }
    public void setDescription(String description) {
        this.description = description;
    }
    public String getCategory() {
        return category;
    }
    public void setCategory(String category) {
        if (CategoryManager.getCategories().contains(category)) {
            this.category = category;
        } else {
            throw new IllegalArgumentException("Category does not exist.");
        }
    }
    public String getPriority() {
        return priority;
    }
    public void setPriority(String priority) {
        if (PriorityManager.getPriorities().contains(priority)) {
            this.priority = priority;
        } else {
            throw new IllegalArgumentException("Priority does not exist.");
        }
    }
    public LocalDate getDeadline() {
        return deadline;
    }
    public void setDeadline(LocalDate deadline) {
        if(deadline.isBefore(LocalDate.now())){
            throw new IllegalArgumentException("Deadline can't be set before the present.");
        }
        else{   this.deadline = deadline; }
    }
    public Status getStatus() {
        return status;
    }
    public void setStatus(Status status) {
        this.status = status;
        if (status == Status.Completed) {
            ReminderManager.removeRemindersForTask(this);
        }
    }
}

