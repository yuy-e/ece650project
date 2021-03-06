# Generated by Django 4.0.1 on 2022-03-13 23:05

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='Color',
            fields=[
                ('color_id', models.AutoField(primary_key=True, serialize=False)),
                ('name', models.CharField(max_length=256)),
            ],
        ),
        migrations.CreateModel(
            name='State',
            fields=[
                ('state_id', models.AutoField(primary_key=True, serialize=False)),
                ('name', models.CharField(max_length=256)),
            ],
        ),
        migrations.CreateModel(
            name='Team',
            fields=[
                ('team_id', models.AutoField(primary_key=True, serialize=False)),
                ('name', models.CharField(max_length=256)),
                ('win', models.IntegerField()),
                ('losses', models.IntegerField()),
                ('color_id', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='orm.color')),
                ('state_id', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='orm.state')),
            ],
        ),
        migrations.CreateModel(
            name='Player',
            fields=[
                ('player_id', models.AutoField(primary_key=True, serialize=False)),
                ('uniform_num', models.IntegerField()),
                ('first_name', models.CharField(max_length=256)),
                ('last_name', models.CharField(max_length=256)),
                ('mpg', models.IntegerField()),
                ('ppg', models.IntegerField()),
                ('rpg', models.IntegerField()),
                ('apg', models.IntegerField()),
                ('spg', models.FloatField()),
                ('bpg', models.FloatField()),
                ('team_id', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='orm.team')),
            ],
        ),
    ]
