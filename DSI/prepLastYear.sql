--1
with ids1 as (
	select distinct a.rid
	from z_year_field_journal yfj
	join z_article ar on yfj.jid = ar.jid and yfj.year = ar.year
	join z_article_author aa on ar.aid = aa.aid
	join z_author a on aa.rid = a.rid
	where yfj.year = 2017 and yfj.ranking = 'Decil'
	group by a.rid, a.name
	having count(distinct ar.aid) >= 15
),
ids2 as (
	select distinct a.rid
	from z_year_field_journal yfj
	join z_article ar on yfj.jid = ar.jid and yfj.year = ar.year
	join z_article_author aa on ar.aid = aa.aid
	join z_author a on aa.rid = a.rid
	where yfj.year = 2018 and yfj.ranking = 'Decil'
	group by a.rid, a.name
	having count(distinct ar.aid) >= 15
),
ids3 as (
	select distinct a.rid
	from z_year_field_journal yfj
	join z_article ar on yfj.jid = ar.jid and yfj.year = ar.year
	join z_article_author aa on ar.aid = aa.aid
	join z_author a on aa.rid = a.rid
	where yfj.year = 2019 and yfj.ranking = 'Decil'
	group by a.rid, a.name
	having count(distinct ar.aid) >= 15
),
ids4 as (
	select distinct a.rid
	from z_year_field_journal yfj
	join z_article ar on yfj.jid = ar.jid and yfj.year = ar.year
	join z_article_author aa on ar.aid = aa.aid
	join z_author a on aa.rid = a.rid
	where yfj.year = 2020 and yfj.ranking = 'Decil'
	group by a.rid, a.name
	having count(distinct ar.aid) >= 15
)

select a.rid, a.name
from z_author a
join z_article_author aa on a.rid = aa.rid
join z_article ar on aa.aid = ar.aid
where a.rid in (select rid from ids1)
and a.rid in (select rid from ids2)
and a.rid in (select rid from ids3)
and a.rid in (select rid from ids4)
group by a.rid, a.name;

--2
with author1 as (
	select ar.aid
	from z_author a
	join z_article_author aa on a.rid = aa.rid
	join z_article ar on aa.aid = ar.aid
	where a.name = 'Baca, Radim'
),
author2 as (
	select ar.aid
	from z_author a
	join z_article_author aa on a.rid = aa.rid
	join z_article ar on aa.aid = ar.aid
	where a.name = 'Kratky, Michal'
)

select a.rid, a.name
from z_author a
join z_article_author aa on a.rid = aa.rid
join z_article ar on aa.aid = ar.aid
where ar.aid in (select * from author1)
and ar.aid in (select * from author2)
and a.name != 'Baca, Radim' and a.name != 'Kratky, Michal'
group by a.rid, a.name
order by a.rid;

--3
with pocty as (
	select ar.aid, ar.name, count(distinct aa.rid) author_count
	from z_article ar
	join z_year_field_journal yfj on ar.jid = yfj.jid and ar.year = yfj.year
	join z_field_ford ff on yfj.fid = ff.fid
	join z_field_of_science fos on ff.sid = fos.sid
	join z_article_author aa on ar.aid = aa.aid
	where fos.name = 'Natural sciences'
	group by ar.aid, ar.name
)

select *
from pocty p
where p.author_count >= all (select author_count from pocty);

--1
with clanky as (
	select i.iid i_iid, i.name i_name, a.rid a_rid, a.name a_name,
	count(distinct ar.aid) clanky_pocet
	from z_institution i
	join z_article_institution ai on i.iid = ai.iid
	join z_article ar on ai.aid = ar.aid
	join z_year_field_journal yfj on ar.jid = yfj.jid and ar.year = yfj.year
	join z_article_author aa on ar.aid = aa.aid
	join z_author a on aa.rid = a.rid
	where i.town like 'Praha%' and yfj.ranking = 'Decil'
	group by i.iid, i.name, a.rid, a.name
)

select *
from clanky c1
where c1.clanky_pocet >= all
(select c2.clanky_pocet from clanky c2 where c1.i_iid = c2.i_iid)
order by c1.clanky_pocet desc;

--2
with isntIn as (
	select distinct a.name
	from z_author a
	join z_article_author aa on a.rid = aa.rid
	join z_article ar on aa.aid = ar.aid
	join z_journal j on ar.jid = j.jid
	where j.issn = '2169-3536'
	group by a.name
)

select distinct a.rid, a.name
from z_author a
join z_article_author aa on a.rid = aa.rid
join z_article ar on aa.aid = ar.aid
join z_journal j on ar.jid = j.jid
where j.issn = '0004-3702' and a.name not in (select i.name from isntIn i)
group by a.rid, a.name
order by a.rid;

--3
select ff.fid, ff.name,
(
	select count(distinct ar.aid) from z_article ar
	join z_year_field_journal yfj on ar.jid = yfj.jid and ar.year = yfj.year
	where ff.fid = yfj.fid

) article_count,
(
	select count(distinct ar.aid) from z_article ar
	join z_year_field_journal yfj on ar.jid = yfj.jid and ar.year = yfj.year
	where yfj.ranking = 'Decil' and ff.fid = yfj.fid
) article_count_decil,
(
	select count(distinct ar.aid) from z_article ar
	join z_year_field_journal yfj on ar.jid = yfj.jid and ar.year = yfj.year
	join z_article_institution ai on ar.aid = ai.aid
	join z_institution i on ai.iid = i.iid
	where i.name = 'Vysoká škola báòská - Technická univerzita Ostrava' and ff.fid = yfj.fid
) article_count_VSB
from z_field_ford ff
join z_field_of_science fos on ff.sid = fos.sid
where fos.name = 'Engineering and Technology'
group by ff.fid, ff.name
order by ff.name;

--1
with autori as (
	select distinct aa.rid
	from z_field_ford ff
	join z_year_field_journal yfj on ff.fid = yfj.fid
	join z_article ar on yfj.jid = ar.jid and ar.year = yfj.year
	join z_article_author aa on ar.aid = aa.aid
	where ff.name = '2.1 Civil engineering'
),
zurnaly as (
select fid
from z_year_field_journal j
join z_article a on a.jid = j.jid and a.year = j.year
join z_article_author aa on a.aid = aa.aid
where aa.rid in (select * from autori)
)

select *
from z_field_ford ff
where ff.fid not in (select * from zurnaly);

--2
with ic as (
	select ar.aid, ar.name, count(distinct i.iid) institution_count
	from z_article ar
	join z_article_institution ai on ar.aid = ai.aid
	join z_institution i on ai.iid = i.iid
	join z_year_field_journal yfj on ar.jid = yfj.jid and ar.year = yfj.year
	where yfj.ranking = 'Decil'
	group by ar.aid, ar.name
)

select ic.aid, ic.name, ic.institution_count
from ic
where ic.institution_count >= all (select institution_count from ic);

--3
with authors1 as (
	select distinct aa.rid
	from z_article_author aa
	join z_article ar on aa.aid = ar.aid
	join z_year_field_journal yfj on ar.jid = yfj.jid and ar.year = yfj.year
	join z_field_ford ff on yfj.fid = ff.fid
	where yfj.ranking = 'Decil' and ff.name = '1.6 Biological Sciences'
),
authors2 as (
	select distinct aa.rid
	from z_article_author aa
	join z_article ar on aa.aid = ar.aid
	join z_year_field_journal yfj on ar.jid = yfj.jid and ar.year = yfj.year
	join z_field_ford ff on yfj.fid = ff.fid
	where yfj.ranking = 'Decil' and ff.name = '6.4 Arts'
)

select distinct a.rid, a.name
from z_author a
join z_article_author aa on a.rid = aa.rid
and aa.rid in (select * from authors1)
and aa.rid in (select * from authors2);

--1
with cte as (
	select ai.iid, yfj.fid, count(distinct ar.aid) clanky
	from z_field_of_science fos
	join z_field_ford ff on fos.sid = ff.sid
	join z_year_field_journal yfj on yfj.fid = ff.fid
	join z_article ar on yfj.jid = ar.jid and yfj.year = ar.year
	join z_article_institution ai on ar.aid = ai.aid
	where fos.name = 'Natural Sciences' and yfj.ranking = 'Decil'
	group by ai.iid, yfj.fid
	having count(distinct ar.aid) >= 10
)

select ff.name, count(distinct cte.iid) pocet_instituci
from z_field_of_science fos
join z_field_ford ff on fos.sid = ff.sid
join z_year_field_journal yfj on yfj.fid = ff.fid
left join cte on cte.fid = ff.fid
where fos.name = 'Natural Sciences' and yfj.ranking = 'Decil'
group by ff.name