package ntua.medialab.medialabassistant;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.time.LocalDate;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class ReminderManager {
    private static List<Reminder> reminders = null;
    private static final String FILE_NAME = "medialab/reminders.json";
    public static void ensureInitialized() {
        if (reminders == null) {
            reminders = loadReminders();
            if (reminders == null) {
                reminders = new ArrayList<>();
            }
        }
    }
    public static void scheduleReminderOneDayBefore(Task task) {
        LocalDate deadline = task.getDeadline();
        if (deadline != null) {
            LocalDate reminderDate = deadline.minusDays(1);
            if (!hasReminderForTask(task, reminderDate)) {
                reminders.add(new Reminder(task, reminderDate));
                System.out.println("Reminder scheduled 1 day before for task: " + task.getTitle());
            } else {
                System.out.println("Reminder already exists for task: " + task.getTitle() + " on date: " + reminderDate);
            }
        }
    }

    // Schedule a reminder for 1 week before the deadline
    public static void scheduleReminderOneWeekBefore(Task task) {
        LocalDate deadline = task.getDeadline();
        if (deadline != null) {
            LocalDate reminderDate = deadline.minusDays(7);
            if (!hasReminderForTask(task, reminderDate)) {
                reminders.add(new Reminder(task, reminderDate));
                System.out.println("Reminder scheduled 1 week before for task: " + task.getTitle());
            } else {
                System.out.println("Reminder already exists for task: " + task.getTitle() + " on date: " + reminderDate);
            }
        }
    }
    // Schedule a reminder for 1 month before the deadline
    public static void scheduleReminderOneMonthBefore(Task task) {
        LocalDate deadline = task.getDeadline();
        if (deadline != null) {
            LocalDate reminderDate = deadline.minusMonths(1);
            if (!hasReminderForTask(task, reminderDate)){
                reminders.add(new Reminder(task, reminderDate));
                System.out.println("Reminder scheduled 1 month before for task: " + task.getTitle());
            } else {
                System.out.println("Reminder already exists for task: " + task.getTitle() + " on date: " + reminderDate);
            }}
    }
    // Schedule a reminder for a user-defined number of days before the deadline
    public static void scheduleReminderUserDefined(Task task, int daysBefore) {
        LocalDate deadline = task.getDeadline();
        if (deadline != null) {
            LocalDate reminderDate = deadline.minusDays(daysBefore);
            if (!hasReminderForTask(task, reminderDate)) {
                reminders.add(new Reminder(task, reminderDate));
                System.out.println("Custom reminder scheduled for task: " + task.getTitle() + " on date: " + reminderDate);
            } else {
                System.out.println("Reminder already exists for task: " + task.getTitle() + " on date: " + reminderDate);
            }
        }
    }
    public static void removeReminder(Reminder reminder) {
        if (reminder == null) return;
        reminder.cancel();
        Iterator<Reminder> iterator = reminders.iterator();
        while (iterator.hasNext()) {
            Reminder currentReminder = iterator.next();
            if (currentReminder.getTask().equals(reminder.getTask()) &&
                    currentReminder.getReminderDate().isEqual(reminder.getReminderDate())) {
                iterator.remove();
                break;
            }
        }
    }
    // Remove reminders for a specific task
    public static void removeRemindersForTask(Task task) {
        Iterator<Reminder> iterator = reminders.iterator();
        while (iterator.hasNext()) {
            Reminder reminder = iterator.next();
            if (reminder.getTask().equals(task)) {
                reminder.cancel();
                iterator.remove();
            }
        }
    }
    public static boolean hasReminderForTask(Task task, LocalDate reminderDate) {
        return reminders.stream()
                .anyMatch(reminder -> reminder.getTask().equals(task) && reminder.getReminderDate().isEqual(reminderDate));
    }
    private static void saveReminders() {
        JSONArray jsonArray = new JSONArray();
        for (Reminder reminder : reminders) {
            JSONObject jsonObject = new JSONObject();
            jsonObject.put("taskTitle", reminder.getTask().getTitle());
            jsonObject.put("reminderDate", reminder.getReminderDate().toString());
            jsonObject.put("trigger", reminder.getTrigger());
            jsonArray.put(jsonObject);
        }
        try (FileWriter file = new FileWriter(FILE_NAME)) {
            file.write(jsonArray.toString(4));
        } catch (IOException e) {
            System.err.println("Error saving reminders: " + e.getMessage());
        }
    }
    public static List<Reminder> loadReminders() {
        if (!Files.exists(Paths.get(FILE_NAME))) {
            return null;
        }
        try {
            String content = new String(Files.readAllBytes(Paths.get(FILE_NAME)));
            JSONArray jsonArray = new JSONArray(content);
            List<Reminder> loadedReminders = new ArrayList<>();
            for (int i = 0; i < jsonArray.length(); i++) {
                JSONObject jsonObject = jsonArray.getJSONObject(i);
                String taskTitle = jsonObject.getString("taskTitle");
                LocalDate reminderDate = LocalDate.parse(jsonObject.getString("reminderDate"));
                boolean trigger = jsonObject.getBoolean("trigger");

                Task task = TaskManager.getTasks().stream()
                        .filter(t -> t.getTitle().equals(taskTitle))
                        .findFirst()
                        .orElse(null);
                if (task == null || task.getDeadline().isBefore(reminderDate)) {
                    System.out.println("Skipping invalid reminder for task: " + taskTitle);
                    continue;
                }

                Reminder reminder = new Reminder(task, reminderDate);
                if (trigger) {
                    reminder.fireTrigger();
                }
                loadedReminders.add(reminder);
            }
            return loadedReminders;
        } catch (IOException e) {
            System.err.println("No saved reminders found or error loading: " + e.getMessage());
            return null;
        }
    }
    public static void cancelAllReminders() {
        for (Reminder reminder : reminders) {
            reminder.cancel();
        }
        reminders.clear();
    }
    public static void setReminders(){
        saveReminders();
    }
    // Get all reminders
    public static List<Reminder> getReminders() {
        return reminders;
    }
}