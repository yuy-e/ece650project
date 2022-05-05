import os

os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'mysql.settings')

import django
django.setup()

from orm.models import Color, State, Team, Player

def loadColorData():
    f = open('orm/data/color.txt')
    for line in f.readlines():
        color_id, name = line.split(' ')
        color = Color.objects.get_or_create(name=name[:-1]) # remove \n
    f.close()
    return

def loadStateData():
    f = open('orm/data/state.txt')
    for line in f.readlines():
        state_id, name = line.split(' ')
        state = State.objects.get_or_create(name=name[:-1])
    f.close()
    return

def loadTeamData():
    f = open('orm/data/team.txt')
    for line in f.readlines():
        team_id, name, state_id, color_id, wins, losses = line.split(' ')
        team = Team.objects.get_or_create(name=name, state_id=State.objects.get(state_id=state_id), color_id=Color.objects.get(color_id=color_id), wins=wins, losses=losses)
    f.close()
    return

def loadPlayerData():
    f = open('orm/data/player.txt')
    for line in f.readlines():
        player_id, team_id, uniform_num, first_name, last_name, mpg, ppg, rpg, apg, spg, bpg = line.split(' ')
        player = Player.objects.get_or_create(team_id=Team.objects.get(team_id=team_id), uniform_num=uniform_num, first_name=first_name, last_name=last_name, mpg=mpg, ppg=ppg, rpg=rpg, apg=apg, spg=spg, bpg=bpg)
    f.close()
    return

def main():
    loadColorData()
    loadStateData()
    loadTeamData()
    loadPlayerData()

if __name__ == "__main__":
    main()