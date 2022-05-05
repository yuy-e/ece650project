from django.db import models

# Create your models here.
class State(models.Model):
    state_id = models.AutoField(primary_key = True)
    name = models.CharField(max_length = 256)
    """
    @classmethod
    def create(cls, name):
        state = cls(name = name)
        return state
    """
    def __str__(self):
        return str(self.state_id) + " " + self.name

class Color(models.Model):
    color_id = models.AutoField(primary_key = True)
    name = models.CharField(max_length = 256)

    """
    @classmethod
    def create(cls, name):
        color = cls(name = name)
        return color
        # color.save()
    """
    def __str__(self):
        return str(self.color_id) + " " + self.name

class Team(models.Model):
    team_id = models.AutoField(primary_key = True)
    name = models.CharField(max_length = 256)
    state_id = models.ForeignKey(State, on_delete=models.CASCADE)
    color_id = models.ForeignKey(Color, on_delete=models.CASCADE)
    wins = models.IntegerField()
    losses = models.IntegerField()
    """
    @classmethod
    def create(cls, name, state_id, color_id, wins, losses):
        team = cls(name = name, state_id = state_id, color_id = color_id, wins = wins, losses = losses)
        return team
    """
    def __str__(self):
        return str(self.team_id) + " " + self.name + " " + str(self.state_id.state_id) + " " + str(self.color_id.color_id) + " " + str(self.wins) + " " + str(self.losses)

class Player(models.Model):
    player_id = models.AutoField(primary_key = True)
    team_id = models.ForeignKey(Team, on_delete=models.CASCADE)
    uniform_num = models.IntegerField()
    first_name = models.CharField(max_length=256)
    last_name = models.CharField(max_length=256)
    mpg = models.IntegerField()
    ppg = models.IntegerField()
    rpg = models.IntegerField()
    apg = models.IntegerField()
    spg = models.FloatField()
    bpg = models.FloatField()
    """
    @classmethod
    def create(cls, team_id, uniform_num, first_name, last_name, mpg, ppg, rpg, apg, spg, bpg):
        player = cls(team_id = team_id, uniform_num = uniform_num, first_name = first_name, last_name= last_name, mpg = mpg, ppg = ppg, rpg = rpg, apg = spg, spg = spg, bpg = bpg)
        return player
    """

    def __str__(self):
        return str(self.player_id) + " " + str(self.team_id.team_id) + " " + str(self.uniform_num) + " " + self.first_name + " " + self.last_name + " " + str(self.mpg) + " " + str(self.ppg) + " " + str(self.rpg) + " " + str(self.apg) + " " + str(self.spg) + " " + str(self.bpg)


