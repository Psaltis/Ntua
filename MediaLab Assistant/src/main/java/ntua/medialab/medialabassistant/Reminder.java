package ntua.medialab.medialabassistant;

import java.time.LocalDate;
import java.time.temporal.ChronoUnit;
import java.util.Timer;
import java.util.TimerTask;

public class Reminder {
    private Task task;
    private LocalDate reminderDate;
    private Boolean trigger;
    private Timer timer;
    public Reminder(Task task, LocalDate reminderDate) {
        this.task = task;
        this.reminderDate = reminderDate;
        this.timer = new Timer();
        this.trigger = false;

        scheduleReminder();
    }
    private void scheduleReminder() {
        long delay = ChronoUnit.MILLIS.between(LocalDate.now().atStartOfDay(), reminderDate.atStartOfDay());
        if (delay > 0) {
            timer.schedule(new ReminderTask(), delay);
        } else {
            // If the reminder date is in the past, cancel the timer
            cancel();
        }
    }
    private class ReminderTask extends TimerTask {
        @Override
        public void run() {
            if (task.getStatus() != Task.Status.Completed) {
                System.out.println("Reminder: Task '" + task + "' is due soon!");
            } else {
                cancel();
            }
        }
    }
    public LocalDate getReminderDate(){
        return reminderDate;
    }
    public boolean getTrigger(){
        return trigger;
    }
    public void fireTrigger(){
        trigger = true;
    }
    public void cancel() {
        timer.cancel();
        timer.purge();
    }
    public Task getTask(){return task;}
}