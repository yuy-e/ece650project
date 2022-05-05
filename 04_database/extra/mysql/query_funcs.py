import os

os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'mysql.settings')

import django
django.setup()

from orm.models import Color, State, Team, Player

def query1(use_mpg,  min_mpg,  max_mpg, use_ppg,  min_ppg,  max_ppg, use_rpg,  min_rpg,  max_rpg, use_apg,  min_apg,  max_apg, use_spg,  min_spg,  max_spg, use_bpg,  min_bpg,  max_bpg):
    all = Player.objects.all()
    if use_mpg:
       all = all.filter(mpg__gte = min_mpg, mpg__lte = max_mpg)
    if use_ppg:
       all = all.filter(ppg__gte = min_ppg, ppg__lte = max_ppg)
    if use_rpg:
       all = all.filter(rpg__gte = min_rpg, rpg__lte = max_rpg)
    if use_apg:
       all = all.filter(apg__gte = min_apg, apg__lte = max_apg)
    if use_spg:
       all = all.filter(spg__gte = min_spg, spg__lte = max_spg)
    if use_bpg:
       all = all.filter(bpg__gte = min_bpg, bpg__lte = max_bpg)
    print("PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG")
    for player in all:
        print(player)
    return


def query2(team_color):
    all = Team.objects.filter(color_id__name = team_color)
    print("NAME")
    for team in all:
        print(team.name)
    return

def query3(team_name):
    all = Player.objects.filter(team_id__name = team_name).order_by('-ppg')
    print("FIRST_NAME LAST_NAME")
    for player in all:
        print(player.first_name, player.last_name)
    return

def query4(team_state, team_color):
    all = Player.objects.filter(team_id__state_id__name = team_state).filter(team_id__color_id__name = team_color)
    print("FIRST_NAME LAST_NAME UNIFORM_NUM")
    for player in all:
        print(player.first_name, player.last_name, player.uniform_num)
    return

def query5(num_wins):
    all = Player.objects.filter(team_id__wins__gt = num_wins)
    print("FIRST_NAME LAST_NAME NAME WINS")
    for player in all:
        print(player.first_name, player.last_name, player.team_id.name, player.team_id.wins)
    return


